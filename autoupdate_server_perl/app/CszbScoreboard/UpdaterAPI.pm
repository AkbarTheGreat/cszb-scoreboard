=pod

A library which has a method for each return value of the updater API, separated from web routes.  All methods should return a hashref, suitable for use as a json object.

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

package CszbScoreboard::UpdaterAPI;
use 5.022;

use Moose;
use lib '../..';
use CszbScoreboard::ReleaseLibrary;
use List::AllUtils qw(none any);
use Mojo::Exception qw(raise);
our @VALID_API_VERSIONS = qw(1.0);

our %DISPATCH_TREE = (
    latest       => sub {latest(@_)},
    versions     => sub {versions(@_)},
    version_info => sub {version_info(@_)},
);

sub api_base {
    my ($self) = @_;
    # If the API changes dramatically and/or in a way that's not mutually
    # backwards-compatible, this should fork into a dispatcher that goes to the
    # version requested by the client.  Also make sure to add a new version to
    # @VALID_API_VERSIONS, above.
    return (api_version => '1.0');
}

sub _version_info_worker {
    my ($self, $version, $mojo) = @_;
    my $lib = CszbScoreboard::ReleaseLibrary->instance();
    return {$self->api_base(),
       version => $version,
       releases => $lib->releases($version, $mojo->app->log),
    };
}
 
sub versions {
    my ($self, $mojo) = @_;
    my $lib = CszbScoreboard::ReleaseLibrary->instance();
    return {$self->api_base(), 'versions' => $lib->versions($mojo->app->log)};
}

sub version_info {
    my ($self, $mojo) = @_;
    my $version = $mojo->param('version');
    return $self->_version_info_worker($version, $mojo);
}

sub latest {
    my ($self, $mojo) = @_;
    my $lib = CszbScoreboard::ReleaseLibrary->instance();
    return $self->_version_info_worker($lib->versions($mojo->app->log)->[-1], $mojo);
}

sub dispatch {
    my ($self, $mojo, $route) = @_;
    my $api_version = $mojo->param('api_version');
    $api_version //= '1.0'; # Default to 1.0 if nothing is passed in.
    $mojo->app->log->debug('REQ Ver: |' . $api_version . '|');
    $mojo->app->log->debug('VAL Ver: |' . $_ . '|') for @VALID_API_VERSIONS;
    if (none {$_ eq $api_version} @VALID_API_VERSIONS) {
        raise 'CszBoston::UpdaterAPI::InvalidAPI', 'Invalid API version: ' . $api_version;
    }
    if (none {$_ eq $route} keys %DISPATCH_TREE) {
        raise 'CszBoston::UpdaterAPI::InvalidMethod', 'Invalid Method: ' . $route;
    }
    return $DISPATCH_TREE{$route}->($self, $mojo);
}

1;
