/*
test/AutoUpdateTest.cpp: Tests for util/AutoUpdate

Copyright 2019-2020 Tracy Beck

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
#include "test/mocks/MockHttpReader.h"
#include "util/AutoUpdate.h"

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

namespace cszb_scoreboard::test {

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
  std::string json = R"({
"name":   "1.2.3",
"assets": [
   {
      "label":  "PlatformName",
      "size":   42,
      "browser_download_url": "https://www.google.com"
   }
]
})";
  HttpResponse testReturn{"", std::vector(json.begin(), json.end())};
  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read).WillRepeatedly(Return(testReturn));
  auto updater = std::make_unique<AutoUpdate>(std::move(reader));
  EXPECT_TRUE(updater->checkForUpdate("0.0.0"));
  EXPECT_TRUE(updater->checkForUpdate("1.1.3"));
  EXPECT_TRUE(updater->checkForUpdate("1.2.2"));
  EXPECT_TRUE(updater->checkForUpdate("0.3.3"));
}

TEST(AutoUpdateTest, NoNewVersionFound) {
  std::string json = R"({
"name":   "1.2.3",
"assets": [
   {
      "label":  "PlatformName",
      "size":   42,
      "browser_download_url": "https://www.google.com"
   }
]
})";
  HttpResponse testReturn{"", std::vector(json.begin(), json.end())};
  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read).WillRepeatedly(Return(testReturn));
  auto updater = std::make_unique<AutoUpdate>(std::move(reader));
  EXPECT_FALSE(updater->checkForUpdate("99999.0.0"));
  EXPECT_FALSE(updater->checkForUpdate("1.2.3"));
  EXPECT_FALSE(updater->checkForUpdate("2.0.0"));
  EXPECT_FALSE(updater->checkForUpdate("1.3.0"));
}

TEST(AutoUpdateTest, VersionDownloads) {
  std::string json = R"({
"name":   "1.2.3",
"assets": [
   {
      "label":  "PlatformName",
      "size":   255,
      "browser_download_url": "https://test-download"
   }
]
})";
  std::vector<char> fake_data;
  for (int i = 0; i < 256; i++) {
    fake_data.emplace_back(i);
  }
  HttpResponse versionReturn{"", std::vector(json.begin(), json.end())};
  HttpResponse updateReturn{"", fake_data};
  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read(HasSubstr("api.github.com")))
      .WillOnce(Return(versionReturn));
  EXPECT_CALL(*reader, read(HasSubstr("test-download")))
      .WillRepeatedly(Return(updateReturn));
  auto updater = std::make_unique<AutoUpdate>(std::move(reader));

  // We need to ask for an update before it'll ever work anyway, so assert on it
  // in case it goes wrong, although NewVersionFound properly tests this.
  EXPECT_TRUE(updater->checkForUpdate("0.5.0", "PlatformName"));
  std::vector<char> update_data;
  EXPECT_TRUE(updater->downloadUpdate(&update_data));
  EXPECT_EQ(update_data.size(), 255);
}

#ifdef SCOREBOARD_INTEGRATION_TEST
// This tests that communication with github actually works, so we should test it sparingly.
TEST(AutoUpdateTest, LiveServerTest) {
  // We need to ask for an update before it'll ever work anyway, so assert on it
  // in case it goes wrong, although NewVersionFound properly tests this.
  EXPECT_TRUE(AutoUpdate::getInstance()->checkForUpdate("0.0.0", "Win64"));
  std::vector<char> update_data;
  EXPECT_TRUE(AutoUpdate::getInstance()->downloadUpdate(&update_data));
  EXPECT_GT(update_data.size(), 4000);
}
#endif // SCOREBOARD_INTEGRATION_TEST

}  // namespace cszb_scoreboard::test
