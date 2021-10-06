/*
util/Log.h: Logging methods.  Redirect to wxWidgets logging in normal execution.
Disables logging in tests entirely by default.  To enable logging in tests (to
stderr instead of wxLog), configure Cmake with
-DCMAKE_CXX_FLAGS="-DSCOREBOARD_ENABLE_LOGGING".

Copyright 2021 Tracy Beck

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

#ifdef SCOREBOARD_TESTING
#ifdef SCOREBOARD_ENABLE_LOGGING
#define LogDebug(...)             \
  {                               \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n");        \
  }
#else  // SCOREBOARD_ENABLE_LOGGING
#define LogDebug(...)
#endif  // SCOREBOARD_ENABLE_LOGGING
#else   // SCOREBOARD_TESTING
#include <wx/wx.h>
#define LogDebug(...) wxLogDebug(__VA_ARGS__)
#endif  // SCOREBOARD_TESTING
