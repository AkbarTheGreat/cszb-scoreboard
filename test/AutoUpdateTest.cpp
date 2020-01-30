/*
test/AutoUpdateTest.cpp: Tests for util/AutoUpdate

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

#include <vector>

#include "gtest/gtest.h"
#include "util/AutoUpdate.h"

namespace cszb_scoreboard {
namespace test {

TEST(AutoUpdateTest, VersionComparisons) {
  EXPECT_EQ(Version("1.2.3"), Version("1.2.3"));
  EXPECT_NE(Version("1.2.3"), Version("3.2.1"));

  EXPECT_GT(Version("1.1.1"), Version("1.1.0"));
  EXPECT_GT(Version("1.1.1"), Version("1.0.1"));
  EXPECT_GT(Version("1.1.1"), Version("0.1.1"));

  EXPECT_GE(Version("1.1.1"), Version("1.1.1"));
  EXPECT_GE(Version("1.1.1"), Version("1.1.0"));
  EXPECT_GE(Version("1.1.1"), Version("1.0.1"));
  EXPECT_GE(Version("1.1.1"), Version("0.1.1"));

  EXPECT_LT(Version("1.1.0"), Version("1.1.1"));
  EXPECT_LT(Version("1.0.1"), Version("1.1.1"));
  EXPECT_LT(Version("0.1.1"), Version("1.1.1"));

  EXPECT_LE(Version("1.1.1"), Version("1.1.1"));
  EXPECT_LE(Version("1.1.0"), Version("1.1.1"));
  EXPECT_LE(Version("1.0.1"), Version("1.1.1"));
  EXPECT_LE(Version("0.1.1"), Version("1.1.1"));

  // Handle versions with extra data at the end.
  EXPECT_EQ(Version("1.2.3"), Version("1.2.3_extra_text"));
  EXPECT_GT(Version("1.2.3"), Version("1.2.0_extra_text"));
  EXPECT_LT(Version("1.2.0"), Version("1.2.3_extra_text"));
}

TEST(AutoUpdateTest, NewVersionFound) {
  EXPECT_TRUE(AutoUpdate::getInstance()->checkForUpdate("0.0.0"));
}

TEST(AutoUpdateTest, NoNewVersionFound) {
  EXPECT_FALSE(AutoUpdate::getInstance()->checkForUpdate("99999.0.0"));
}

TEST(AutoUpdateTest, VersionDownloads) {
  // We need to ask for an update before it'll ever work anyway, so assert on it
  // in case it goes wrong, although NewVersionFound properly tests this.
  EXPECT_TRUE(AutoUpdate::getInstance()->checkForUpdate("0.0.0"));
  std::vector<char> update_data;
  EXPECT_TRUE(AutoUpdate::getInstance()->downloadUpdate(update_data));
  EXPECT_GT(update_data.size(), 4000);
}

}  // namespace test
}  // namespace cszb_scoreboard
