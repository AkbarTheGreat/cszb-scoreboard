/*
test/FilesystemPathTest.cpp: Tests for util/FilesystemPath

Copyright 2022-2023 Tracy Beck

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

#include <filesystem>  // for path::preferred_separator
#include <string>      // for allocator, string

#include "gtest/gtest.h"          // for AssertionResult, Test, Message, Tes...
#include "util/FilesystemPath.h"  // for FilesystemPath

#if defined(_WIN32) && !defined(SCOREBOARD_APPLE_IMPL)
const char *ROOT = "C:\\test";
const char *ROOT_WITH_SEP = "C:\\test\\";
const char *RELATIVE_PATH_IN_ROOT = "path1\\file1.ext";
const char *ABS_PATH_IN_ROOT = "C:\\test\\path1\\file1.ext";
const char *ABS_PATH_OUT_OF_ROOT = "C:\\test2\\path1\\file1.ext";
#else   // #if defined(_WIN32) && !defined(SCOREBOARD_APPLE_IMPL)
// Treat Linux or Apple the same for testing
const char *ROOT = "/test";
const char *ROOT_WITH_SEP = "/test/";
const char *RELATIVE_PATH_IN_ROOT = "path1/file1.ext";
const char *ABS_PATH_IN_ROOT = "/test/path1/file1.ext";
const char *ABS_PATH_OUT_OF_ROOT = "/test2/path1/file1.ext";
#endif  // #if defined(_WIN32) && !defined(SCOREBOARD_APPLE_IMPL)

#ifdef SCOREBOARD_APPLE_IMPL
#define TEST_NAME FilesystemPathTest_MacOS
#else  // #ifdef SCOREBOARD_APPLE_IMPL
#define TEST_NAME FilesystemPathTest
#endif  // #ifdef SCOREBOARD_APPLE_IMPL

const char *CURRENT_BINARY;

// Has it's own main to get the current file from argv, as a means of finding a
// valid file on the filesystem.
auto main(int argc, char **argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  CURRENT_BINARY = argv[0];
  return RUN_ALL_TESTS();
}

namespace cszb_scoreboard::test {

TEST(TEST_NAME, AbsolutePathConversions) {
  EXPECT_EQ(FilesystemPath::absolutePath(ROOT, RELATIVE_PATH_IN_ROOT),
            ABS_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::absolutePath(ROOT, ABS_PATH_OUT_OF_ROOT),
            ABS_PATH_OUT_OF_ROOT);
}

TEST(TEST_NAME, RelativePathConversions) {
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT, RELATIVE_PATH_IN_ROOT),
            RELATIVE_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT, ABS_PATH_IN_ROOT),
            RELATIVE_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT, ABS_PATH_OUT_OF_ROOT),
            ABS_PATH_OUT_OF_ROOT);
}

TEST(TEST_NAME, TrailingPathSeparator) {
  EXPECT_EQ(FilesystemPath::absolutePath(ROOT_WITH_SEP, RELATIVE_PATH_IN_ROOT),
            ABS_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::absolutePath(ROOT_WITH_SEP, ABS_PATH_OUT_OF_ROOT),
            ABS_PATH_OUT_OF_ROOT);
  EXPECT_EQ(
      FilesystemPath::mostRelativePath(ROOT_WITH_SEP, RELATIVE_PATH_IN_ROOT),
      RELATIVE_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT_WITH_SEP, ABS_PATH_IN_ROOT),
            RELATIVE_PATH_IN_ROOT);
  EXPECT_EQ(
      FilesystemPath::mostRelativePath(ROOT_WITH_SEP, ABS_PATH_OUT_OF_ROOT),
      ABS_PATH_OUT_OF_ROOT);
}

#ifndef SCOREBOARD_APPLE_IMPL
// This functionality is known lacking in MacOS, all files return true for
// existence.
// TODO(#39): Fix this for MacOS
TEST(TEST_NAME, FileExistence) {
  // The path for the currently running test -- we know this exists, so we use
  // it for existence testing.
  FilesystemPath abs_test_path = FilesystemPath(std::string(CURRENT_BINARY));
  abs_test_path.make_preferred();
  std::string relative_root = abs_test_path.string().substr(
      0,
      abs_test_path.string().find_last_of(FilesystemPath::preferred_separator) +
          1);
  FilesystemPath relative_test_path = FilesystemPath(
      FilesystemPath::mostRelativePath(relative_root, abs_test_path.string()));

  // Ensure our relative path is non-empty.
  EXPECT_NE(relative_root, "");
  EXPECT_NE(relative_test_path, abs_test_path);

  // Simple absolute path to test exists.
  EXPECT_TRUE(abs_test_path.existsWithRoot(""));
  // Root is ignored for absolute paths.
  EXPECT_TRUE(abs_test_path.existsWithRoot(ROOT));

  // Test exists for correct relative path.
  EXPECT_TRUE(relative_test_path.existsWithRoot(relative_root));
  // Test not found for empty root for relative path.
  EXPECT_FALSE(relative_test_path.existsWithRoot(""));
  // Test not found for invalid root for relative path.
  EXPECT_FALSE(relative_test_path.existsWithRoot(ROOT));

  // Now test some with a file that doesn't exist at all.
  FilesystemPath bad_path = FilesystemPath(ABS_PATH_IN_ROOT);
  EXPECT_FALSE(bad_path.existsWithRoot(""));
}
#endif  // #ifndef SCOREBOARD_APPLE_IMPL

TEST(TEST_NAME, TitleNameTests) {
  FilesystemPath path("path-with-dashes");
  EXPECT_EQ(path.titleName(), "Path With Dashes");
  path = FilesystemPath("path_with_underscores");
  EXPECT_EQ(path.titleName(), "Path With Underscores");
  path = FilesystemPath("path-with-the-mixed-caps");
  EXPECT_EQ(path.titleName(), "Path With the Mixed Caps");
  path = FilesystemPath("the-first-word-should-be-caps");
  EXPECT_EQ(path.titleName(), "The First Word Should Be Caps");
  path = FilesystemPath("last-word-should-be-caps-on");
  EXPECT_EQ(path.titleName(), "Last Word Should Be Caps On");
  path = FilesystemPath("kEeP-wEiRd-CaPs");
  EXPECT_EQ(path.titleName(), "KEeP WEiRd CaPs");
  path = FilesystemPath("singleWord");
  EXPECT_EQ(path.titleName(), "SingleWord");
  path = FilesystemPath("removes-extensions.jpg");
  EXPECT_EQ(path.titleName(), "Removes Extensions");
}

}  // namespace cszb_scoreboard::test
