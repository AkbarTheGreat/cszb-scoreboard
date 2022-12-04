/*
test/FilesystemPathTest.cpp: Tests for util/FilesystemPath

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

#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult
#include <gtest/gtest.h>            // IWYU pragma: keep

#include <memory>  // for allocator

#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST
#include "util/FilesystemPath.h"    // for FilesystemPath

#ifdef _WIN32
const char *ROOT = "C:\\test";
const char *ROOT_WITH_SEP = "C:\\test\\";
const char *RELATIVE_PATH_IN_ROOT = "path1\\file1.ext";
const char *ABS_PATH_IN_ROOT = "C:\\test\\path1\\file1.ext";
const char *ABS_PATH_OUT_OF_ROOT = "C:\\test2\\path1\\file1.ext";
#else   // ifdef _WIN32
// Treat Linux or Apple the same for testing
const char *ROOT = "/test";
const char *ROOT_WITH_SEP = "/test/";
const char *RELATIVE_PATH_IN_ROOT = "path1/file1.ext";
const char *ABS_PATH_IN_ROOT = "/test/path1/file1.ext";
const char *ABS_PATH_OUT_OF_ROOT = "/test2/path1/file1.ext";
#endif  // ifdef _WIN32

namespace cszb_scoreboard::test {

TEST(FilesystemPathTest, AbsolutePathConversions) {
  EXPECT_EQ(FilesystemPath::absolutePath(ROOT, RELATIVE_PATH_IN_ROOT),
            ABS_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::absolutePath(ROOT, ABS_PATH_OUT_OF_ROOT),
            ABS_PATH_OUT_OF_ROOT);
}

TEST(FilesystemPathTest, RelativePathConversions) {
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT, RELATIVE_PATH_IN_ROOT),
            RELATIVE_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT, ABS_PATH_IN_ROOT),
            RELATIVE_PATH_IN_ROOT);
  EXPECT_EQ(FilesystemPath::mostRelativePath(ROOT, ABS_PATH_OUT_OF_ROOT),
            ABS_PATH_OUT_OF_ROOT);
}

TEST(FilesystemPathTest, TrailingPathSeparator) {
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

}  // namespace cszb_scoreboard::test
