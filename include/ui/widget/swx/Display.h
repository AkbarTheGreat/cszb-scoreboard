/*
ui/widget/swx/Display.h: A wrapper around wxDisplay.

Copyright 2021-2025 Tracy Beck

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

#include <wx/display.h>

namespace cszb_scoreboard::swx {

class Display {
 public:
  virtual auto IsPrimary() -> bool = 0;
  virtual auto GetGeometry() -> wxRect = 0;
};

class DisplayImpl : public Display, wxDisplay {
 public:
  explicit DisplayImpl(unsigned int index = 0) : wxDisplay(index) {}

  auto IsPrimary() -> bool override { return wxDisplay::IsPrimary(); }
  auto GetGeometry() -> wxRect override { return wxDisplay::GetGeometry(); }
};

}  // namespace cszb_scoreboard::swx
