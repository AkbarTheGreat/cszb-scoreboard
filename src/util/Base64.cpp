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

namespace cszb_scoreboard {

auto Base64::encode(const void *data_buffer, int64_t buffer_len)
    -> std::string {
  return "";
}

auto Base64::decode(const std::string &data, std::vector<char> *bin_out) -> int64_t {
  return -1;
}

}  // namespace cszb_scoreboard
