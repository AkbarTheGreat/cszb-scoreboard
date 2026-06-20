=pod

A library which manages the actual interaction with the library of releases as they're stored on disk.

Copyright 2023 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=cut

package CszbScoreboard::ReleaseLibrary;
use 5.022;

use DateTime;
use DDP;
use MooseX::Singleton;
use MooseX::Privacy;
use Redis;
use CszbScoreboard::Config;

has '_cache' => (
    is      => 'ro',
    isa     => 'Redis',
    lazy    => 1,
    default => sub {return Redis->new(
                    server => 'updater-cache:6379',
                    reconnect => 60,
                    every => 100_000
    );},
);

has '_version_cache_last_update' => (
    is     => 'rw',
    isa => 'DateTime',
    traits => [qw/Private/],
);

has '_version_cache' => (
    is     => 'rw',
    isa => 'ArrayRef[Str]',
    traits => [qw/Private/],
);

has '_latest_release_cache' => (
    is     => 'rw',
    isa => 'ArrayRef[HashRef]',
    traits => [qw/Private/],
);

sub releases {
    my ($self, $version, $log) = @_;
    return _gather_releases($version);
}

sub versions {
    my ($self, $log) = @_;
    return $self->_gather_versions($log);
}

sub _compare_versions {
    my ($a, $b) = @_;
    my @a = split /\./, $a;
    my @b = split /\./, $b;
    if (@b > @a) {
        return -1;
    }
    if (@a > @b) {
        return 1;
    }
    while (@a) {
        my $a_prime = shift @a;
        my $b_prime = shift @b;
        $a_prime =~ s/_.*//;
        $b_prime =~ s/_.*//;
        if ($b_prime > $a_prime) {
            return -1;
        }
        if ($a_prime > $b_prime) {
            return 1;
        }
    }
    return $a cmp $b;
}

sub _gather_releases {
    my ($self, $version, $log) = @_;

    my @releases;
    my $version_dir = CszbScoreboard::Config->instance()->release_path() . '/' . $version;
    my $ver_dh;
    unless (opendir($ver_dh, $version_dir)) {
        $log->error('Can\'t open ' . $version_dir . ': ' . $!);
        return;
    }
    while (readdir $ver_dh) {
        next if /^\.\.?$/;
        push @releases, {'platform' => $_};
    }
    closedir $ver_dh;

    for my $rel_hash (@releases) {
        my $plat = $rel_hash->{'platform'};
        my $platform_dir = $version_dir . '/' . $plat;
        my $plat_dh;
        $rel_hash->{'files'} = {};
        unless (opendir($plat_dh, $platform_dir)) {
            $log->error('Can\'t open ' . $platform_dir . ': ' . $!);
            return;
        }
        while (readdir $plat_dh) {
            next if /^\.\.?$/;
            $rel_hash->{'files'}{$_} = {size => (stat $platform_dir . '/' . $_)[7]};
        }
        closedir $plat_dh;
    }

    return \@releases;
}

sub _sorted_versions {
    my ($log, @versions) = @_;
    return [sort {_compare_versions($a, $b, $log)} @versions];
}
 
sub _gather_versions {
    my ($self, $log) = @_;
    if ($self->_cache()->exists('versions')) {
       $log->debug('Using cached known versions');
       return _sorted_versions($log, $self->_cache()->smembers('versions'));
    }

    $log->debug('Refreshing known versions');
    my @versions;
    my $dir = CszbScoreboard::Config->instance()->release_path();
    my $release_dir;
    unless (opendir($release_dir, $dir)) {
        $log->error('Can\'t open ' . $dir . ': ' . $!);
        return;
    }
    while (readdir $release_dir) {
        next if /^\.\.?$/;
        push @versions, $_;
    }
    closedir $release_dir;

    $self->_cache()->sadd('versions', @versions);
    $self->_cache()->expire('versions', CszbScoreboard::Config->instance()->release_cache_minutes()*60);

    return _sorted_versions($log, @versions);
}

sub _is_latest {
    my ($self, $version, $log) = @_;
    return 1 if $version eq $self->versions($log)->[-1];
    return undef;
}

1;
