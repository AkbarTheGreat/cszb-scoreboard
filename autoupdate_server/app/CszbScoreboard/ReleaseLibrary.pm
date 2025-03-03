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
    builder   => '_gather_versions',
);
 
sub _gather_versions {
    my ($self) = @_;
    $self->_set_versions(['0.5', '1.0']);
}

1;
