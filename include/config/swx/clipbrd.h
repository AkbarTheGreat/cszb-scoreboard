/*
config/swx/clipbrd.h:  A simple wrapper around wx's clipbrd header.  Wrappers
like this exist to make it easier to spot where unexpected wx direct
dependencies are creeping into our codebase.

Copyright 2021-2024 Tracy Beck

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

#pragma once

#include <wx/clipbrd.h>  // IWYU pragma: export
