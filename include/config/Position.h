/*
config/Position.h: A few structs which help with passing around positions or
sizes, easily convertable to wxSize and wxPosition or proto::Rect.

Copyright 2019-2025 Tracy Beck

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

#include <wx/gdicmn.h>  // for wxPoint, wxSize

#include <cstdint>  // for int64_t

namespace cszb_scoreboard {
namespace proto {
class Rectangle;
}  // namespace proto

struct Position {
 public:
  int64_t x, y;
  [[nodiscard]] auto toWx() const -> wxPoint {
    return {static_cast<int>(x), static_cast<int>(y)};
  }
  static auto fromWx(const wxPoint& wx) -> Position;
  [[nodiscard]] auto isContainedIn(const proto::Rectangle& box) const -> bool;
  auto operator==(const Position& rhs) const -> bool;
  auto operator!=(const Position& rhs) const -> bool;
  auto operator-() const -> Position;
  auto operator+(const Position& b) const -> Position;
  auto operator-(const Position& b) const -> Position;
  auto operator*(const Position& b) const -> Position;
  auto operator/(const Position& b) const -> Position;
  auto operator%(const Position& b) const -> Position;
};

struct Size {
 public:
  int64_t width, height;
  [[nodiscard]] auto toWx() const -> wxSize {
    return {static_cast<int>(width), static_cast<int>(height)};
  }
  static auto fromWx(const wxSize& wx) -> Size;
  auto operator==(const Size& rhs) const -> bool;
  auto operator!=(const Size& rhs) const -> bool;
  auto operator-() const -> Size;
  auto operator+(const Size& b) const -> Size;
  auto operator-(const Size& b) const -> Size;
  auto operator*(const Size& b) const -> Size;
  auto operator*(double b) const -> Size;
  auto operator/(const Size& b) const -> Size;
  auto operator%(const Size& b) const -> Size;
};

}  // namespace cszb_scoreboard
