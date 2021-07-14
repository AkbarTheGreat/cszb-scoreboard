/*
config/Position.h: A few structs which help with passing around positions or
sizes, easily convertable to wxSize and wxPosition or proto::Rect.

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

#include <wx/gdicmn.h>

namespace cszb_scoreboard {

struct Position {
 public:
  int32_t x, y;
  [[nodiscard]] auto toWx() const -> wxPoint { return wxPoint(x, y); }
  static auto fromWx(const wxPoint& wx) -> Position;
};

struct Size {
 public:
  int32_t width, height;
  [[nodiscard]] auto toWx() const -> wxSize { return wxSize(width, height); }
  static auto fromWx(const wxSize& wx) -> Size;
};

}  // namespace cszb_scoreboard
