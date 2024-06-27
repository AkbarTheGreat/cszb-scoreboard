
/*
test/unit/meta/TempFilesystemTest.cpp: Tests for the class which creates
auto-destructing filesystems for testing.

Copyright 2024 Tracy Beck

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

#include <filesystem>                  // for exists, operator/, path, temp_...
#include <memory>                      // for allocator, unique_ptr, make_un...

#include "gmock/gmock.h"               // for MakePredicateFormatterFromMatcher
#include "gtest/gtest.h"               // for AssertionResult, Message, Test...
#include "test/util/TempFilesystem.h"  // for TempFilesystem

namespace cszb_scoreboard::test {

using ::testing::Return;
using ::testing::StartsWith;

class TempFilesystemTest : public ::testing::Test {
 protected:
  std::unique_ptr<TempFilesystem> fs;

  TempFilesystemTest() = default;
  ~TempFilesystemTest() override = default;

  void SetUp() override { fs = std::make_unique<TempFilesystem>(); }

  void TearDown() override { fs.reset(); }
};

TEST_F(TempFilesystemTest, RandomRoot) {
  EXPECT_THAT(fs->getRoot().string(),
              StartsWith(std::filesystem::temp_directory_path().string()));
  EXPECT_TRUE(std::filesystem::exists(fs->getRoot()));
}

TEST_F(TempFilesystemTest, RootIsCreatedAndDestroyed) {
  auto root = fs->getRoot();
  EXPECT_TRUE(std::filesystem::exists(root));
  fs.reset();
  EXPECT_FALSE(std::filesystem::exists(root))
      << root.string() << " was not properly deleted.";
}

TEST_F(TempFilesystemTest, SubdirCreation) {
  auto root = fs->getRoot();
  fs->createSubdir("subdir");
  EXPECT_TRUE(std::filesystem::exists(root / "subdir"));
}

TEST_F(TempFilesystemTest, FileCreation) {
  auto root = fs->getRoot();
  fs->createFile("testfile.png", "foo");
  EXPECT_TRUE(std::filesystem::exists(root / "testfile.png"));
}

TEST_F(TempFilesystemTest, NestedFileCreation) {
  auto root = fs->getRoot();
  fs->createSubdir("subdir");
  fs->createFile("subdir/testfile.png", "foo");
  EXPECT_TRUE(std::filesystem::exists(root / "subdir" / "testfile.png"));
}

TEST_F(TempFilesystemTest, ComplicatedStructureIsDestroyed) {
  auto root = fs->getRoot();
  fs->createSubdir("testsub");
  fs->createSubdir("testsub/nested");
  fs->createSubdir("testsub2");
  fs->createSubdir("emptydir");
  fs->createFile("testfile1", "testfile1 content");
  fs->createFile("testsub/nested/testfile2", "testfile2 content");
  fs->createFile("testsub2/testfile3", "testfile3 content");
  fs.reset();
  EXPECT_FALSE(std::filesystem::exists(root))
      << root.string() << " was not properly deleted.";
}

}  // namespace cszb_scoreboard::test
