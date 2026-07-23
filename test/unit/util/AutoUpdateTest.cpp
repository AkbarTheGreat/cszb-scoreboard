/*
test/AutoUpdateTest.cpp: Tests for util/AutoUpdate

Copyright 2019-2026 Tracy Beck

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

#include <gtest/gtest.h>  // for Test, CmpHelperGE, CmpHe...

#include <memory>   // for allocator, unique_ptr
#include <string>   // for string
#include <utility>  // for move
#include <vector>   // for vector

#include "test/mocks/util/MockHttpReader.h"  // for MockHttpReader
#include "test/mocks/util/MockSingleton.h"   // for MockSingleton
#include "util/AutoUpdate.h"                 // for Version, AutoUpdate
#include "util/HttpReader.h"                 // for HttpResponse
#include "util/Singleton.h"                  // for SingletonClass
#include "version_info.pb.h"

#define TEST_STUB_SINGLETON
#include "test/mocks/Stubs.h"

// IWYU pragma: no_include "gmock/gmock.h"
// IWYU pragma: no_include "gtest/gtest.h"
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

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

TEST(AutoUpdateTest, AutoupdateServerFound) {
  proto::GetLatestResponse latest_resp;
  latest_resp.mutable_latest()->set_name("1.2.3");
  std::string latest_str = latest_resp.SerializeAsString();
  HttpResponse latestReturn{"", std::vector<char>(latest_str.begin(), latest_str.end())};

  proto::GetUpdateURLsResponse urls_resp;
  urls_resp.add_urls("https://autoupdate.cszb-scoreboard.org/download");
  std::string urls_str = urls_resp.SerializeAsString();
  HttpResponse urlsReturn{"", std::vector<char>(urls_str.begin(), urls_str.end())};

  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read(HasSubstr("api/versions/latest")))
      .WillRepeatedly(Return(latestReturn));
  EXPECT_CALL(*reader, read(HasSubstr("update_path")))
      .WillRepeatedly(Return(urlsReturn));

  MockSingleton singleton;
  auto updater = testObject(std::move(reader), &singleton);
  EXPECT_TRUE(updater->checkForUpdate("0.0.0", "Win64"));
  EXPECT_TRUE(updater->checkForUpdate("1.1.3", "Win64"));
  EXPECT_FALSE(updater->checkForUpdate("1.2.3", "Win64"));
  EXPECT_FALSE(updater->checkForUpdate("2.0.0", "Win64"));
}

TEST(AutoUpdateTest, FallbackToGithubWhenServerFails) {
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
  HttpResponse githubReturn{"", std::vector<char>(json.begin(), json.end())};
  HttpResponse serverError{"Connection refused", {}};

  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read(HasSubstr("autoupdate.cszb-scoreboard.org")))
      .WillRepeatedly(Return(serverError));
  EXPECT_CALL(*reader, read(HasSubstr("api.github.com")))
      .WillRepeatedly(Return(githubReturn));

  MockSingleton singleton;
  auto updater = testObject(std::move(reader), &singleton);
  EXPECT_TRUE(updater->checkForUpdate("0.0.0", "PlatformName"));
  EXPECT_FALSE(updater->checkForUpdate("1.2.3", "PlatformName"));
}

TEST(AutoUpdateTest, VersionDownloadsViaAutoupdateServer) {
  proto::GetLatestResponse latest_resp;
  latest_resp.mutable_latest()->set_name("1.2.3");
  std::string latest_str = latest_resp.SerializeAsString();
  HttpResponse latestReturn{"", std::vector<char>(latest_str.begin(), latest_str.end())};

  proto::GetUpdateURLsResponse urls_resp;
  urls_resp.add_urls("https://autoupdate.cszb-scoreboard.org/download");
  std::string urls_str = urls_resp.SerializeAsString();
  HttpResponse urlsReturn{"", std::vector<char>(urls_str.begin(), urls_str.end())};

  std::vector<char> fake_data = {'a', 'b', 'c', 'd', '\0'};
  HttpResponse downloadReturn{"", fake_data};

  auto reader = std::make_unique<MockHttpReader>();
  EXPECT_CALL(*reader, read(HasSubstr("api/versions/latest")))
      .WillOnce(Return(latestReturn));
  EXPECT_CALL(*reader, read(HasSubstr("update_path")))
      .WillOnce(Return(urlsReturn));
  EXPECT_CALL(*reader, read(HasSubstr("download")))
      .WillOnce(Return(downloadReturn));

  MockSingleton singleton;
  auto updater = testObject(std::move(reader), &singleton);
  EXPECT_TRUE(updater->checkForUpdate("0.5.0", "Win64"));

  std::vector<char> update_data;
  EXPECT_TRUE(updater->downloadUpdate(&update_data));
  EXPECT_EQ(update_data.size(), 4);
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
