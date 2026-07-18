/*
test/unit/util/FontUtilTest.cpp: Tests for util/FontUtil

Copyright 2026 Tracy Beck

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

#include <gtest/gtest.h>

#include <memory>  // for allocator

#include "config/Position.h"  // for Size
#include "util/FontUtil.h"    // for FontUtil

// IWYU pragma: no_include "gmock/gmock.h"
// IWYU pragma: no_include "gtest/gtest.h"
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard::test {

TEST(FontUtilTest, ScaleFactorCalculations) {
  Size display_size{1920, 1080};

  // 1080 * 10 / 75 = 144
  EXPECT_EQ(FontUtil::scaleFactor(display_size, 10), 144);
  EXPECT_EQ(FontUtil::scaleFactor(display_size, 10.0), 144);

  // 1080 * 0 / 75 = 0, but scaleFactor clamps to 1 if it is 0
  EXPECT_EQ(FontUtil::scaleFactor(display_size, 0), 1);
  EXPECT_EQ(FontUtil::scaleFactor(display_size, 0.0), 1);

  // 1080 * 20.5 / 75 = 295.2 -> 295
  EXPECT_EQ(FontUtil::scaleFactor(display_size, 20.5), 295);
}

}  // namespace cszb_scoreboard::test
