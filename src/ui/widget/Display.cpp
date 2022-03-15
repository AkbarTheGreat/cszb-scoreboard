/*
ui/widget/Display.cpp: A representation of a physical display.  This is notable
within the widgets classes in that it owns it's underlying wx object and
construct it itself in normal cases, and has an injectible constructor for
testing.

Copyright 2021-2022 Tracy Beck

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

#include "ui/widget/Display.h"

#include <array>

#include "util/ProtoUtil.h"  // for ProtoUtil

namespace cszb_scoreboard {

constexpr int DEBUG_STRING_BUFFER_SIZE = 256;

namespace proto {
class Rectangle;
}  // namespace proto

Display ::Display(uint32_t index)
    : Display(dynamic_pointer_cast<swx::Display, swx::DisplayImpl>(
          std::make_shared<swx::DisplayImpl>(index))) {}

void Display::geometry(proto::Rectangle* rectangle) {
  ProtoUtil::protoRct(_wx->GetGeometry(), rectangle);
}

auto Display::debugString() -> std::string {
  wxRect geo = _wx->GetGeometry();
  std::array<char, DEBUG_STRING_BUFFER_SIZE> format_str;
  snprintf(format_str.data(), format_str.size(), "%dx%d", geo.GetWidth(),
           geo.GetHeight());
  return std::string(format_str.data());
}

}  // namespace cszb_scoreboard
