=pod

A singleton that manages the config for the updater.

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

package CszbScoreboard::Config;
use MooseX::Singleton;
 
has 'access_log' => (
    is     => 'rw',
    isa => 'Str',
    default => '/logs/access.log',
);

has 'release_path' => (
    is     => 'rw',
    isa => 'Str',
    default => '/releases',
);

# Cut down on file IO by only checking for new releases every N minutes.
has 'release_cache_minutes' => (
    is     => 'rw',
    isa => 'Num',
    default => $ENV{'SCOREBOARD_AU_CACHE_TIME'} // 120,
);

1;
