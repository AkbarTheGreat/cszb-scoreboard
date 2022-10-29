/*
test/StringUtilTest.cpp: Tests for util/StringUtil

Copyright 2022 Tracy Beck

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

#include "util/StringUtil.h"  // for StringUtil

// extern wxColourDatabase *wxTheColourDatabase;

namespace cszb_scoreboard::test {

TEST(StringUtilTest, ConvertsPositiveIntegersToStrings) {
  EXPECT_EQ(StringUtil::intToString(1), "1");
  EXPECT_EQ(StringUtil::intToString(20), "20");
  EXPECT_EQ(StringUtil::intToString(1234567890L), "1234567890");
}

TEST(StringUtilTest, ConvertsPositiveIntegersFromStrings) {
  EXPECT_EQ(StringUtil::stringToInt("1"), 1);
  EXPECT_EQ(StringUtil::stringToInt("20"), 20);
  EXPECT_EQ(StringUtil::stringToInt("1234567890"),
            1234567890L);  // NOLINT(readability-magic-numbers)
}

TEST(StringUtilTest, ConvertsZeroBothDirections) {
  EXPECT_EQ(StringUtil::intToString(0), "0");
  EXPECT_EQ(StringUtil::stringToInt("0"), 0);
}

TEST(StringUtilTest, ConvertsNegativeIntegersToStrings) {
  EXPECT_EQ(StringUtil::intToString(-1), "-1");
  EXPECT_EQ(StringUtil::intToString(-20), "-20");
  EXPECT_EQ(StringUtil::intToString(-1234567890L), "-1234567890");
}

TEST(StringUtilTest, ConvertsNegativeIntegersFromStrings) {
  EXPECT_EQ(StringUtil::stringToInt("-1"), -1);
  EXPECT_EQ(StringUtil::stringToInt("-20"), -20);
  EXPECT_EQ(StringUtil::stringToInt("-1234567890"),
            -1234567890L);  // NOLINT(readability-magic-numbers)
}

TEST(StringUtilTest, ZeroPaddingWorksAsExpected) {
  EXPECT_EQ(StringUtil::intToString(1, 0), "1");
  EXPECT_EQ(StringUtil::intToString(1, 1), "1");
  EXPECT_EQ(StringUtil::intToString(1, 2), "01");
  EXPECT_EQ(StringUtil::intToString(1, 5), "00001");
  EXPECT_EQ(StringUtil::intToString(-1, 0), "-1");
  EXPECT_EQ(StringUtil::intToString(-1, 1), "-1");
  EXPECT_EQ(StringUtil::intToString(-1, 2), "-1");
  EXPECT_EQ(StringUtil::intToString(-1, 3), "-01");
  EXPECT_EQ(StringUtil::intToString(-1, 4), "-001");
}

}  // namespace cszb_scoreboard::test
