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
use CszbScoreboard::Config;

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
    if ($self->_is_latest($version, $log)) {
        return $self->_latest_release_cache();
    }
    return _gather_releases($version);
}

sub versions {
    my ($self, $log) = @_;
    $self->_gather_versions($log);
    return $self->_version_cache();
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
            $rel_hash->{'files'}{$_} = {size => 0};
        }
        closedir $plat_dh;
    }

    return \@releases;
}
 
sub _gather_versions {
    my ($self, $log) = @_;
    unless ($self->_version_cache_last_update()) {
        $self->_version_cache_last_update(DateTime->now);
        $self->_version_cache_last_update()->subtract(minutes => (CszbScoreboard::Config->instance()->release_cache_minutes()*2));
    }

    my $refresh = $self->_version_cache_last_update()->clone();

    $refresh->add(minutes => CszbScoreboard::Config->instance()->release_cache_minutes());
    if (DateTime->now < $refresh) {
        $log->debug('Using cached known versions');
        return;
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

    $self->_version_cache_last_update(DateTime->now);
    @versions = sort {_compare_versions($a, $b, $log)} @versions;
    $self->_version_cache(\@versions);
    $self->_latest_release_cache($self->_gather_releases($versions[-1], $log));
}

sub _is_latest {
    my ($self, $version, $log) = @_;
    return 1 if $version eq $self->versions($log)->[-1];
    return undef;
}

1;
