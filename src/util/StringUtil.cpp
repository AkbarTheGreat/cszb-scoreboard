/*
util/StringUtil.cpp: Convenience methods for dealing with wxStrings.

Copyright 2019-2022 Tracy Beck

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

#include "util/StringUtil.h"

#include <wx/chartype.h>  // for wxT
#include <wx/string.h>    // for wxString, wxCStrData

#include <array>   // for array
#include <cstdio>  // for snprintf

namespace cszb_scoreboard {

constexpr int32_t FORMAT_BUFFER_SIZE =
    64;  // Only used when zero-padding int-to-string conversions

auto StringUtil::intToString(int value, int zero_pad) -> std::string {
  wxString wxs;
  wxString format = wxT("%d");
  // This is kind of a gross kludge and I don't love it, but it works for any
  // suitably large zero_pad (and anything bigger than ~60 characters of pad
  // seems unnecessary)
  if (zero_pad > 0) {
    std::array<char, FORMAT_BUFFER_SIZE> format_str;
    snprintf(format_str.data(), format_str.size(), "%%0%dd", zero_pad);
    format = wxString(format_str.data());
  }
  wxs.Printf(format, value);
  return {wxs.c_str()};
}

auto StringUtil::stringToInt(const std::string &string, int default_value)
    -> int64_t {
  wxString wxs = string;
  long value = default_value;  // NOLINT(google-runtime-int) Must be long to
                               // match string.ToLong() below.
  if (wxs.IsNumber()) {
    wxs.ToLong(&value);
  }
  return value;
}

}  // namespace cszb_scoreboard
