/*
test/TestUtil.h: Some common utilities to facilite testing.

Copyright 2021-2025 Tracy Beck

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

#include <gtest/gtest.h>  // IWYU pragma: keep for Test

#include "gtest/gtest_pred_impl.h"  // for Test

// Allow us to test that assertions work in debug tests, but since assertions go
// away in release mode, turn EXPECT_ASSERT into a Nop.
#ifdef SCOREBOARD_DEBUG
#define EXPECT_ASSERT(x) EXPECT_DEATH(x, "Assertion")
#else  // SCOREBOARD_DEBUG
#define EXPECT_ASSERT(x)
#endif  // SCOREBOARD_DEBUG

namespace cszb_scoreboard::test {}  // namespace cszb_scoreboard::test
