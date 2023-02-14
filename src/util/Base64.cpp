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

#include <unordered_map>

namespace cszb_scoreboard {

const char ENCODE_MAP[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

auto build_decode_map() -> std::unordered_map<char, char> {
  std::unordered_map<char, char> map;
  for (int i = 0; i < 64; i++) {
    map[ENCODE_MAP[i]] = i;
  }
  return map;
}

// I could initialize this lazily instead of at startup, but it's n=64 sized
// map, so this shouldn't be terribly inefficient to do every time.
const std::unordered_map<char, char> DECODE_MAP = build_decode_map();

auto Base64::encode(const void *data_buffer, int64_t buffer_len)
    -> std::string {
  std::vector<char> output;
  output.reserve(buffer_len * 8 / 6);  // Reserve expected space
  const char *char_buffer = static_cast<const char *>(data_buffer);
  for (int i = 0; i < buffer_len; i++) {
    int32_t idx = (char_buffer[i] & 0xFC) >> 2;  // 11111100, shifted by two
    output.push_back(ENCODE_MAP[idx]);
    idx = (char_buffer[i] & 0x03) << 4;  // 00000011, shifted by four
    if (++i == buffer_len) {
      // Double pad, we're dropping four bits.
      output.push_back(ENCODE_MAP[idx]);
      output.push_back('=');
      output.push_back('=');
      break;
    }
    idx |= (char_buffer[i] & 0xF0) >> 4;  // 11110000, shifted by four
    output.push_back(ENCODE_MAP[idx]);
    idx = (char_buffer[i] & 0x0F) << 2;  // 00001111, shifted by two
    if (++i == buffer_len) {
      // Single pad, we're dropping two bits.
      output.push_back(ENCODE_MAP[idx]);
      output.push_back('=');
      break;
    }
    idx |= (char_buffer[i] & 0xC0) >> 6;  // 11000000, shifted by six
    output.push_back(ENCODE_MAP[idx]);
    idx = (char_buffer[i] & 0x3F);  // 00111111, unshifted
    output.push_back(ENCODE_MAP[idx]);
  }
  output.push_back('\0');
  return std::string(output.data());
}

auto Base64::decode(const std::string &data, std::vector<char> *bin_out)
    -> int64_t {
  bin_out->reserve(data.length() * 6 / 8);  // Reserve expected space
  for (int i = 0; i < data.length(); i++) {
    char byte = DECODE_MAP.at(data[i])
                << 2;  // xx111111 six bits, shifted left by two
    byte |= (DECODE_MAP.at(data[++i]) & 0x30) >>
            4;  // xx110000, First two bits, shifted right by 4
    bin_out->push_back(byte);
    byte = (DECODE_MAP.at(data[i]) & 0x0F)
           << 4;  // xx001111, Last four bits, shifted left by 4
    if (data[++i] == '=') {
      break;
    }
    byte |= (DECODE_MAP.at(data[i]) & 0x3C) >>
            2;  // xx111100, First four bits, shifted right by 2
    bin_out->push_back(byte);
    byte = (DECODE_MAP.at(data[i]) & 0x03)
           << 6;  // xx000011, Last two bits, shifted left by 6
    if (data[++i] == '=') {
      break;
    }
    byte |= DECODE_MAP.at(data[i]);  // xx111111, All six bits, unshifted
    bin_out->push_back(byte);
  }
  return bin_out->size();
}

}  // namespace cszb_scoreboard
