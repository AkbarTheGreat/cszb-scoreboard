/*
ScoreboardCommon.h: This header holds common constants and macros which should
be available anywhere in the program.  It should depend on nothing else
internal.

Copyright 2019-2023 Tracy Beck

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

#include <string>

namespace cszb_scoreboard {

const int DEFAULT_BORDER_SIZE = 5;

const std::string IMAGE_SELECTION_STRING =
    "Image files (bmp, gif, jpeg, png)|*.bmp;*.gif;*.jpg;*.jpeg;*.png";

const std::string LOGO_SELECTION_STRING = "Logo files (png)|*.png";

#define SCOREBOARD_VERSION "1.6.3"

// Evaluates to public: in a test environment, private: in non-test code, to
// hide dependencies as tightly as possible
#ifdef SCOREBOARD_TESTING
#define PUBLIC_TEST_ONLY public:
#else
#define PUBLIC_TEST_ONLY protected:
#endif

}  // namespace cszb_scoreboard
