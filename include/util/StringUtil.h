/*
util/StringUtil.h: Convenience methods for dealing with wxStrings.

Copyright 2019-2021 Tracy Beck

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
#pragma once

#include <wx/string.h>  // for wxString

#include <cstdint>  // for int64_t

namespace cszb_scoreboard {

class StringUtil {
 public:
  static auto intToString(int value) -> std::string;
  static auto stringToInt(const std::string &string, int default_value = 0)
      -> int64_t;
};

}  // namespace cszb_scoreboard
