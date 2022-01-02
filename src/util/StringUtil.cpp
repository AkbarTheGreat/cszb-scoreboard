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

#include <wx/chartype.h>

namespace cszb_scoreboard {

auto StringUtil::intToString(int value) -> std::string {
  wxString wxs;
  wxs.Printf(wxT("%d"), value);
  return std::string(wxs.c_str());
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
