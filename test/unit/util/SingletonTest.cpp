/*
test/unit/util/SingletonTest.cpp: Tests for util/Singleton

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
#include <wx/gdicmn.h>  // for wxTheColourDatabase, wxColourDatabase

#include <memory>  // for allocator

#include "util/Singleton.h"  // for Singleton

// IWYU pragma: no_include "gmock/gmock.h"
// IWYU pragma: no_include "gtest/gtest.h"
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard::test {

TEST(SingletonTest, GettersReturnNonNull) {
  if (wxTheColourDatabase == nullptr) {
    wxTheColourDatabase = new wxColourDatabase();
  }

  Singleton* singleton = Singleton::getInstance();
  ASSERT_NE(singleton, nullptr);

  EXPECT_NE(singleton->displayConfig(), nullptr);
  EXPECT_NE(singleton->generalConfig(), nullptr);
  EXPECT_NE(singleton->persistence(), nullptr);
  EXPECT_NE(singleton->slideShow(), nullptr);
  EXPECT_NE(singleton->teamConfig(), nullptr);
  EXPECT_NE(singleton->timerManager(), nullptr);
  EXPECT_NE(singleton->autoUpdate(), nullptr);
  EXPECT_NE(singleton->teamColors(), nullptr);
  EXPECT_NE(singleton->hotkeyTable(), nullptr);
  EXPECT_NE(singleton->imageLibrary(), nullptr);
}

}  // namespace cszb_scoreboard::test
