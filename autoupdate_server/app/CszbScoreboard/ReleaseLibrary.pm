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

sub versions {
    my ($self, $log) = @_;
    $self->_gather_versions($log);
    return $self->_version_cache();
}

sub _compare_versions {
    my ($a, $b, $log) = @_;
    my @a = split /\./, $a;
    my @b = split /\./, $b;
    $log->debug('A: ' . $a . ' B: ' . $b);
    if (@b > @a) {
        $log->debug('_1');
        return -1;
    }
    if (@a > @b) {
        $log->debug('_2');
        return 1;
    }
    $log->debug('  @A ' . scalar(@a) . " @a");
    while (@a) {
        my $a_prime = shift @a;
        my $b_prime = shift @b;
        $a_prime =~ s/_.*//;
        $b_prime =~ s/_.*//;
        $log->debug('  A`: ' . $a_prime . ' B`: ' . $b_prime);
        if ($b_prime > $a_prime) {
            $log->debug('_3');
            return -1;
        }
        if ($a_prime > $b_prime) {
            $log->debug('_4');
            return 1;
        }
    }
    return $a cmp $b;
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
}

1;
