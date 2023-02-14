/*
util/Base64.cpp: Encode/decode data in Base64.  There are a few options out
there, but I didn't want to try to mix licenses with my MIT license, so I wrote
my own from scratch to avoid this issue.

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

#include "util/Base64.h"

#include <array>
#include <unordered_map>

namespace cszb_scoreboard {

constexpr int DATA_WIDTH = 64;

const std::array<char, DATA_WIDTH> ENCODE_MAP{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};


auto build_decode_map() -> std::unordered_map<char, char> {
  std::unordered_map<char, char> map;
  for (int i = 0; i < DATA_WIDTH; i++) {
    map[ENCODE_MAP[i]] = i;
  }
  return map;
}

constexpr int BITS_PER_BYTE = 8;
constexpr int BITS_PER_64 = 6;
constexpr int BYTE_FIRST_SIX = 0xFC;       // 1111_1100
constexpr int BYTE_SHIFTR_FIRST_SIX = 2;   // shifts right two
constexpr int BYTE_LAST_TWO = 0x03;        // 0000_0011
constexpr int BYTE_SHIFTL_LAST_TWO = 4;    // shifts left four
constexpr int BYTE_FIRST_FOUR = 0xF0;      // 1111_0000
constexpr int BYTE_SHIFTR_FIRST_FOUR = 4;  // shifts right four
constexpr int BYTE_LAST_FOUR = 0x0F;       // 0000_1111
constexpr int BYTE_SHIFTL_LAST_FOUR = 2;   // shifts left two
constexpr int BYTE_FIRST_TWO = 0xC0;       // 1100_0000
constexpr int BYTE_SHIFTR_FIRST_TWO = 6;   // shifts right six
constexpr int BYTE_LAST_SIX = 0x3F;        // 0011_1111 (does not shift)

constexpr int B64_SHIFTL_FULL_SIX = 2;    // shifts left two (not masked)
constexpr int B64_FIRST_TWO = 0x30;       // xx11_0000
constexpr int B64_SHIFTR_FIRST_TWO = 4;   // shifts right 4
constexpr int B64_LAST_FOUR = 0x0F;       // xx00_1111
constexpr int B64_SHIFTL_LAST_FOUR = 4;   // shifts left 4
constexpr int B64_FIRST_FOUR = 0x3C;      // xx11_1100
constexpr int B64_SHIFTR_FIRST_FOUR = 2;  // shifts right 2
constexpr int B64_LAST_TWO = 0x03;        // xx00_0011
constexpr int B64_SHIFTL_LAST_TWO = 6;    // shifts left 6

// I could initialize this lazily instead of at startup, but it's n=64 sized
// map, so this shouldn't be terribly inefficient to do every time.
const std::unordered_map<char, char> DECODE_MAP = build_decode_map();

auto Base64::encode(const void *data_buffer, int64_t buffer_len)
    -> std::string {
  std::vector<char> output;
  output.reserve(buffer_len * BITS_PER_BYTE /
                 BITS_PER_64);  // Reserve expected space
  const char *char_buffer = static_cast<const char *>(data_buffer);
  for (int i = 0; i < buffer_len; i++) {
    int32_t idx = (char_buffer[i] & BYTE_FIRST_SIX) >> BYTE_SHIFTR_FIRST_SIX;
    output.push_back(ENCODE_MAP[idx]);
    idx = (char_buffer[i] & BYTE_LAST_TWO) << BYTE_SHIFTL_LAST_TWO;
    if (++i == buffer_len) {
      // Double pad, we're dropping four bits.
      output.push_back(ENCODE_MAP[idx]);
      output.push_back('=');
      output.push_back('=');
      break;
    }
    idx |= (char_buffer[i] & BYTE_FIRST_FOUR) >> BYTE_SHIFTR_FIRST_FOUR;
    output.push_back(ENCODE_MAP[idx]);
    idx = (char_buffer[i] & BYTE_LAST_FOUR) << BYTE_SHIFTL_LAST_FOUR;
    if (++i == buffer_len) {
      // Single pad, we're dropping two bits.
      output.push_back(ENCODE_MAP[idx]);
      output.push_back('=');
      break;
    }
    idx |= (char_buffer[i] & BYTE_FIRST_TWO) >> BYTE_SHIFTR_FIRST_TWO;
    output.push_back(ENCODE_MAP[idx]);
    idx = (char_buffer[i] & BYTE_LAST_SIX);  // unshifted
    output.push_back(ENCODE_MAP[idx]);
  }
  output.push_back('\0');
  return {output.data()};
}

auto Base64::decode(const std::string &data, std::vector<char> *bin_out)
    -> int64_t {
  bin_out->reserve(data.length() * BITS_PER_64 /
                   BITS_PER_BYTE);  // Reserve expected space
  for (int i = 0; i < data.length(); i++) {
    char byte = DECODE_MAP.at(data[i]) << B64_SHIFTL_FULL_SIX;  // (not masked)
    byte |= (DECODE_MAP.at(data[++i]) & B64_FIRST_TWO) >> B64_SHIFTR_FIRST_TWO;
    bin_out->push_back(byte);
    byte = (DECODE_MAP.at(data[i]) & B64_LAST_FOUR) << B64_SHIFTL_LAST_FOUR;
    if (data[++i] == '=') {
      break;
    }
    byte |= (DECODE_MAP.at(data[i]) & B64_FIRST_FOUR) >> B64_SHIFTR_FIRST_FOUR;
    bin_out->push_back(byte);
    byte = (DECODE_MAP.at(data[i]) & B64_LAST_TWO) << B64_SHIFTL_LAST_TWO;
    if (data[++i] == '=') {
      break;
    }
    byte |= DECODE_MAP.at(data[i]);  // All six bits, unmasked, unshifted
    bin_out->push_back(byte);
  }
  return bin_out->size();
}

}  // namespace cszb_scoreboard
