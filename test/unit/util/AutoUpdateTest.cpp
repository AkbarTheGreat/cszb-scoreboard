/*
test/AutoUpdateTest.cpp: Tests for util/AutoUpdate

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

#include <memory>   // for allocator, unique_ptr
#include <string>   // for string
#include <utility>  // for move
#include <vector>   // for vector

#include "gmock/gmock.h"                     // for Return, ReturnAction
#include "gtest/gtest.h"                     // for AssertionResult, Message
#include "test/mocks/util/MockHttpReader.h"  // for MockHttpReader
#include "test/mocks/util/MockSingleton.h"   // for MockSingleton
#include "util/AutoUpdate.h"                 // for Version, AutoUpdate
#include "util/HttpReader.h"                 // for HttpResponse
#include "util/Singleton.h"                  // for SingletonClass

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

namespace cszb_scoreboard::test {

auto testObject(std::unique_ptr<MockHttpReader> reader,
                MockSingleton* singleton) -> std::unique_ptr<AutoUpdate> {
  return std::make_unique<AutoUpdate>(SingletonClass{}, singleton,
                                      std::move(reader));
}

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
  MockSingleton singleton;
  auto updater = testObject(std::move(reader), &singleton);
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
  MockSingleton singleton;
  auto updater = testObject(std::move(reader), &singleton);
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
  std::vector<char> fake_data(256);  // NOLINT(readability-magic-numbers)
  for (int i = 0; i < 256; i++) {    // NOLINT(readability-magic-numbers)
    fake_data[i] = i;
  }
  HttpResponse versionReturn{"", std::vector(json.begin(), json.end())};
  HttpResponse updateReturn{"", fake_data};
  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read(HasSubstr("api.github.com")))
      .WillOnce(Return(versionReturn));
  EXPECT_CALL(*reader, read(HasSubstr("test-download")))
      .WillRepeatedly(Return(updateReturn));
  MockSingleton singleton;
  auto updater = testObject(std::move(reader), &singleton);

  // We need to ask for an update before it'll ever work anyway, so assert on it
  // in case it goes wrong, although NewVersionFound properly tests this.
  EXPECT_TRUE(updater->checkForUpdate("0.5.0", "PlatformName"));
  std::vector<char> update_data;
  EXPECT_TRUE(updater->downloadUpdate(&update_data));
  EXPECT_EQ(update_data.size(), 255);
}

#ifdef SCOREBOARD_INTEGRATION_TEST
// This tests that communication with github actually works, so we should test
// it sparingly.
TEST(AutoUpdateTest, LiveServerTest) {
  MockSingleton singleton;
  AutoUpdate updater(SingletonClass{});
  EXPECT_CALL(singleton, autoUpdate()).WillRepeatedly(Return(&updater));
  EXPECT_TRUE(singleton.autoUpdate()->checkForUpdate("0.0.0", "Win64"));
  std::vector<char> update_data;
  EXPECT_TRUE(singleton.autoUpdate()->downloadUpdate(&update_data));
  EXPECT_GT(update_data.size(), 4000);
}
#endif  // SCOREBOARD_INTEGRATION_TEST

}  // namespace cszb_scoreboard::test
