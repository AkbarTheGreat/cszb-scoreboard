/*
ScoreboardCommon.h: This header holds common constants and macros which should
be available anywhere in the program.  It should depend on nothing else
internal.

Copyright 2019 Tracy Beck

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

namespace cszb_scoreboard {

#define SCOREBOARD_VERSION "0.0.2"

// Evaluates to public: in a test environment, private: in non-test code, to
// hide dependencies as tightly as possible
#ifdef SCOREBOARD_TESTING
#define PUBLIC_TEST_ONLY public:
#else
#define PUBLIC_TEST_ONLY private:
#endif

}  // namespace cszb_scoreboard
