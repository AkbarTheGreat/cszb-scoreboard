/*
test/unit/config/ImageLibrary/CaseOptionalStringTest.cpp: Tests for
config/ImageLibrary's CaseOptionalString class.  The directory structure isn't a
perfect mirror, but I wanted to break out tests for this small helper class into
their own test file for clarity.

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

#include <gtest/gtest.h>

#include "config/ImageLibrary.h"            // for ImageLibrary, ImageSearch...
#include "test/mocks/util/MockSingleton.h"  // for MockSingleton
#include "util/FilesystemPath.h"            // for FilesystemPath
#include "util/Singleton.h"                 // for SingletonClass

namespace cszb_scoreboard::test {

TEST(CaseOptionalStringTest, ConstructorEmptyString) {
  CaseOptionalString cs("");
  EXPECT_EQ("", cs.string());
  EXPECT_EQ("", cs.lower());
}

TEST(CaseOptionalStringTest, ConstructorNonEmptyString) {
  CaseOptionalString cs("Hello");
  EXPECT_EQ("Hello", cs.string());
  EXPECT_EQ("hello", cs.lower());
}

// Test lower() method
TEST(CaseOptionalStringTest, LowerMethod) {
  CaseOptionalString cs("HELLO");
  EXPECT_EQ("hello", cs.lower());
}

// Test string() method
TEST(CaseOptionalStringTest, StringMethod) {
  CaseOptionalString cs("Hello");
  EXPECT_EQ("Hello", cs.string());
}

// Test find() method - exact match
TEST(CaseOptionalStringTest, FindExactMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_EQ(0u, cs.find(CaseOptionalString("Hello World")));

  // Also works case insensitive
  EXPECT_EQ(0u, cs.find(CaseOptionalString("heLLo woRld")));
}

// Test find() method - word boundary match
TEST(CaseOptionalStringTest, FindWordMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_EQ(6u, cs.find(CaseOptionalString("World")));

  // Also works case insensitive
  EXPECT_EQ(6u, cs.find(CaseOptionalString("woRld")));
}

// Test find() method - partial match
TEST(CaseOptionalStringTest, FindPartialMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_EQ(4u, cs.find(CaseOptionalString("o")));

  // Also works case insensitive
  EXPECT_EQ(4u, cs.find(CaseOptionalString("O")));
}

// Test find() method - no match
TEST(CaseOptionalStringTest, FindNoMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_EQ(std::string::npos, cs.find(CaseOptionalString("abc")));
}

// Test substring() method
TEST(CaseOptionalStringTest, SubstringExactMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_TRUE(cs.substring(CaseOptionalString("Hello World")));

  // Also works case insensitive
  EXPECT_TRUE(cs.substring(CaseOptionalString("heLLo woRld")));
}

// Test substring() method - word boundary match
TEST(CaseOptionalStringTest, SubstringWordMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_TRUE(cs.substring(CaseOptionalString("World")));

  // Also works case insensitive
  EXPECT_TRUE(cs.substring(CaseOptionalString("woRld")));
}

// Test substring() method - partial match
TEST(CaseOptionalStringTest, SubstringPartialMatch) {
  CaseOptionalString cs("Hello World");
  EXPECT_TRUE(cs.substring(CaseOptionalString("o")));

  // Also works case insensitive
  EXPECT_TRUE(cs.substring(CaseOptionalString("O")));
}

// Test compare() method - exact match
TEST(CaseOptionalStringTest, CompareExactMatch) {
  CaseOptionalString cs1("Hello");
  CaseOptionalString cs2("Hello");
  EXPECT_EQ(0, cs1.compare(cs2));

  // Comparison is case sensitive, for sort stability.
  CaseOptionalString cs3("heLLo");
  EXPECT_EQ(-1, cs1.compare(cs3));
}

// Test compare() method - partial match
TEST(CaseOptionalStringTest, ComparePartialMatch) {
  CaseOptionalString cs1("Hello");
  CaseOptionalString cs2("Hel");
  EXPECT_LT(0, cs1.compare(cs2));

  // Also works case insensitive
  CaseOptionalString cs3("heL");
  EXPECT_LT(0, cs1.compare(cs3));
}

// Test compare() method - no match
TEST(CaseOptionalStringTest, CompareNoMatch) {
  CaseOptionalString cs1("Hello");
  CaseOptionalString cs2("Goodbye");
  EXPECT_LT(0, cs1.compare(cs2));
}

// Test equality operators
TEST(CaseOptionalStringTest, EqualityOperators) {
  CaseOptionalString cs1("Hello");
  CaseOptionalString cs2("Hello");
  EXPECT_TRUE(cs1 == cs2);
  EXPECT_FALSE(cs1 != cs2);

  CaseOptionalString cs3("Goodbye");
  EXPECT_FALSE(cs1 == cs3);
  EXPECT_TRUE(cs1 != cs3);

  // Comparison is case sensitive, for sort stability.
  CaseOptionalString cs4("heLLo");
  EXPECT_FALSE(cs1 == cs4);
  EXPECT_TRUE(cs1 != cs4);
}

// Test comparison operators
TEST(CaseOptionalStringTest, ComparisonOperators) {
  CaseOptionalString cs1("Hello");
  CaseOptionalString cs2("Hello");
  EXPECT_FALSE(cs1 < cs2);
  EXPECT_TRUE(cs1 <= cs2);
  EXPECT_FALSE(cs1 > cs2);
  EXPECT_TRUE(cs1 >= cs2);

  CaseOptionalString cs3("Goodbye");
  EXPECT_FALSE(cs1 < cs3);
  EXPECT_FALSE(cs1 <= cs3);
  EXPECT_TRUE(cs1 > cs3);
  EXPECT_TRUE(cs1 >= cs3);

  EXPECT_TRUE(cs3 < cs1);
  EXPECT_FALSE(cs3 > cs1);

  // Comparison is case sensitive, for sort stability.
  CaseOptionalString cs4("heLLo");
  EXPECT_TRUE(cs1 < cs4);
  EXPECT_TRUE(cs1 <= cs4);
  EXPECT_FALSE(cs1 > cs4);
  EXPECT_FALSE(cs1 >= cs4);

  EXPECT_FALSE(cs4 < cs1);
  EXPECT_TRUE(cs4 > cs1);
}

}  // namespace cszb_scoreboard::test
