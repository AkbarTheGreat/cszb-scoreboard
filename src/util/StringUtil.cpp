/*
util/StringUtil.cpp: Convenience methods for dealing with wxStrings.

Copyright 2019 Tracy Beck

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

namespace cszb_scoreboard {

wxString StringUtil::intToString(int value) {
  wxString string;
  string.Printf(wxT("%d"), value);
  return string;
}

long StringUtil::stringToInt(wxString string, int default_value) {
  long value = default_value;
  if (string.IsNumber()) {
    string.ToLong(&value);
  }
  return (int)value;
}

}  // namespace cszb_scoreboard