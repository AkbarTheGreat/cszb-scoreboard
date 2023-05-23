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
use Moose;
use lib '../..';
use CszbScoreboard::ReleaseLibrary;
 
sub versions {
    my ($self) = @_;
    my $lib = CszbScoreboard::ReleaseLibrary->instance();
    return {'versions' => $lib->versions()};
}

1;
