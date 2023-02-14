/*
test/unit/util/Base64Test.cpp: Tests for util/Base64

Copyright 2023 Tracy Beck

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

#include <gtest/gtest.h>  // IWYU pragma: keep for GTest

#include "util/Base64.h"

namespace cszb_scoreboard::test {

// Will result in a double-pad, due to being 4  bits shy of a 6 bit boundary.
const std::string TEST_DECODE_1 = "This is a test string.";
// Will result in a single pad, due to being 2 bits off of a 6 bit boundary.
const std::string TEST_DECODE_2 = "My dog's a good dog?";
// Will result in no pad, due to being a multiple of 6 bits.
const std::string TEST_DECODE_3 = "My dog is the best dog!!";

// Test encodes calculated using javascript.
const std::string TEST_ENCODE_1 = "VGhpcyBpcyBhIHRlc3Qgc3RyaW5nLg==";
const std::string TEST_ENCODE_2 = "TXkgZG9nJ3MgYSBnb29kIGRvZz8=";
const std::string TEST_ENCODE_3 = "TXkgZG9nIGlzIHRoZSBiZXN0IGRvZyEh";

TEST(Base64Test, EncodeStrings) {
  EXPECT_EQ(Base64::encode(TEST_DECODE_1.c_str(), TEST_DECODE_1.length()), "");
}

TEST(Base64Test, DecodeStrings) {
  std::vector<char> output;
  EXPECT_EQ(Base64::decode(TEST_ENCODE_1, &output), -1);
}

}  // namespace cszb_scoreboard::test
