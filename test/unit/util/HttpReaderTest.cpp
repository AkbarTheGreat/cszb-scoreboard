/*
test/unit/util/HttpReaderTest.cpp: Tests for util/HttpReader

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

#include <algorithm>  // IWYU pragma: keep for max
#include <string>     // for allocator, string
#include <vector>     // for vector

#include "gmock/gmock.h"                     // for Return, ReturnAction, StrEq
#include "gtest/gtest.h"                     // for Test, AssertionResult
#include "test/mocks/util/MockHttpReader.h"  // for MockHttpReader
#include "util/HttpReader.h"                 // for HttpResponse, HttpReader
// IWYU pragma: no_include <gtest/gtest.h>

namespace cszb_scoreboard::test {

using ::testing::Return;

TEST(HttpReaderTest, DataUrlDecoding) {
  HttpReader reader;
  std::vector<char> result;

  // Base64 for "hello world" is "aGVsbG8gd29ybGQ="
  std::string url = "data:image/png;base64,aGVsbG8gd29ybGQ=";
  EXPECT_TRUE(reader.readBinary(url.c_str(), &result));

  std::string decoded(result.begin(), result.end());
  EXPECT_EQ(decoded, "hello world");
}

TEST(HttpReaderTest, SuccessfulBinaryRead) {
  MockHttpReader reader;

  std::string binary_content = "some binary data";
  HttpResponse normal_response{
      "", std::vector<char>(binary_content.begin(), binary_content.end())};
  normal_response.response.push_back(
      '\0');  // readBinary sheds the trailing null

  EXPECT_CALL(reader, read(testing::StrEq("http://normal.com")))
      .WillOnce(Return(normal_response));

  std::vector<char> result;
  EXPECT_TRUE(reader.readBinary("http://normal.com", &result));
  EXPECT_EQ(std::string(result.begin(), result.end()), "some binary data");
}

TEST(HttpReaderTest, RedirectsToNewLocation) {
  MockHttpReader reader;

  // HTML response matching isRedirect criteria: starts with <html>, size
  // between 50 and 1000, contains href
  std::string html_redirect =
      "<html><body><a href=\"http://redirected.com\">Redirect "
      "link here</a></body></html>";
  html_redirect.push_back('\0');
  HttpResponse redirect_response{
      "", std::vector<char>(html_redirect.begin(), html_redirect.end())};

  std::string final_content = "final data";
  final_content.push_back('\0');
  HttpResponse final_response{
      "", std::vector<char>(final_content.begin(), final_content.end())};

  EXPECT_CALL(reader, read(testing::StrEq("http://start.com")))
      .WillOnce(Return(redirect_response));
  EXPECT_CALL(reader, read(testing::StrEq("http://redirected.com")))
      .WillOnce(Return(final_response));

  std::vector<char> result;
  EXPECT_TRUE(reader.readBinary("http://start.com", &result));
  EXPECT_EQ(std::string(result.begin(), result.end()), "final data");
}

TEST(HttpReaderTest, RedirectLimit) {
  MockHttpReader reader;

  std::string html_redirect =
      "<html><body><a href=\"http://start.com\">Redirect "
      "link here</a></body></html>";
  html_redirect.push_back('\0');
  HttpResponse redirect_response{
      "", std::vector<char>(html_redirect.begin(), html_redirect.end())};

  EXPECT_CALL(reader, read(testing::_))
      .WillRepeatedly(Return(redirect_response));

  std::vector<char> result;
  EXPECT_FALSE(reader.readBinary("http://start.com", &result));
}

TEST(HttpReaderTest, ReadErrorReturnsFalse) {
  MockHttpReader reader;
  HttpResponse error_response{"Connection timed out", {}};

  EXPECT_CALL(reader, read(testing::_)).WillOnce(Return(error_response));

  std::vector<char> result;
  EXPECT_FALSE(reader.readBinary("http://error.com", &result));
}

}  // namespace cszb_scoreboard::test
