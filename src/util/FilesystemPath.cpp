/*
util/FilesystemPath.cpp: In most cases, a simple wrapper around
std::filesystem::path. For cases where that support is unavailable, a simple
stand-in which implements the functionality we need for our application.

Copyright 2020 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "util/FilesystemPath.h"

namespace cszb_scoreboard {

#ifdef __APPLE__
// TODO: Put OSX implementation of our class here.
#endif

}  // namespace cszb_scoreboard
