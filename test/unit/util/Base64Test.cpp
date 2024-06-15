/*
test/unit/util/Base64Test.cpp: Tests for util/Base64

Copyright 2023-2024 Tracy Beck

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

#include <string>  // for string, allocator
#include <vector>  // for vector

#include "gtest/gtest.h"  // for Test, EXPECT_EQ, Message, TestPartResult
#include "util/Base64.h"  // for Base64

namespace cszb_scoreboard::test {

// Will result in no pad, due to being a multiple of 6 bits.
const std::string TEST_DECODE_1 = "My dog is the best dog!!";
// Will result in a single pad, due to being 2 bits off of a 6 bit boundary.
const std::string TEST_DECODE_2 = "My dog's a good dog?";
// Will result in a double-pad, due to being 4  bits shy of a 6 bit boundary.
const std::string TEST_DECODE_3 = "This is a test string.";

// Test encodes calculated using javascript.
const std::string TEST_ENCODE_1 = "TXkgZG9nIGlzIHRoZSBiZXN0IGRvZyEh";
const std::string TEST_ENCODE_2 = "TXkgZG9nJ3MgYSBnb29kIGRvZz8=";
const std::string TEST_ENCODE_3 = "VGhpcyBpcyBhIHRlc3Qgc3RyaW5nLg==";

TEST(Base64Test, EncodeStrings) {
  EXPECT_EQ(Base64::encode(TEST_DECODE_1.c_str(), TEST_DECODE_1.length()),
            TEST_ENCODE_1);
  EXPECT_EQ(Base64::encode(TEST_DECODE_2.c_str(), TEST_DECODE_2.length()),
            TEST_ENCODE_2);
  EXPECT_EQ(Base64::encode(TEST_DECODE_3.c_str(), TEST_DECODE_3.length()),
            TEST_ENCODE_3);
}

TEST(Base64Test, DecodeStrings) {
  std::vector<char> output;
  EXPECT_EQ(Base64::decode(TEST_ENCODE_1, &output), TEST_DECODE_1.length());
  output.push_back('\0');
  EXPECT_EQ(std::string(output.data()), TEST_DECODE_1);
  output.resize(0);
  EXPECT_EQ(Base64::decode(TEST_ENCODE_2, &output), TEST_DECODE_2.length());
  output.push_back('\0');
  EXPECT_EQ(std::string(output.data()), TEST_DECODE_2);
  output.resize(0);
  EXPECT_EQ(Base64::decode(TEST_ENCODE_3, &output), TEST_DECODE_3.length());
  output.push_back('\0');
  EXPECT_EQ(std::string(output.data()), TEST_DECODE_3);
}

}  // namespace cszb_scoreboard::test
