/*
config/Position.cpp: A few structs which help with passing around positions or
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

#include "config/Position.h"

#include "config.pb.h"  // for Rectangle

namespace cszb_scoreboard {

auto Position::fromWx(const wxPoint& wx) -> Position {
  Position pos;
  pos.x = wx.x;
  pos.y = wx.y;
  return pos;
}

auto Position::isContainedIn(const proto::Rectangle& box) const -> bool {
  return (x >= box.x() && y >= box.y() &&
          x <= static_cast<int64_t>(box.x()) + box.width() &&
          y <= static_cast<int64_t>(box.y()) + box.width());
}

auto Position::operator==(const Position& rhs) const -> bool {
  return (this->x == rhs.x && this->y == rhs.y);
}

auto Position::operator!=(const Position& rhs) const -> bool {
  return !(*this == rhs);
}

auto Position::operator-() const -> Position {
  return Position{.x = -this->x, .y = -this->y};
}

auto Position::operator+(const Position& b) const -> Position {
  return Position{.x = this->x + b.x, .y = this->y + b.y};
}

auto Position::operator-(const Position& b) const -> Position {
  return *this + -b;
}

auto Position::operator*(const Position& b) const -> Position {
  return Position{.x = this->x * b.x, .y = this->y * b.y};
}

auto Position::operator/(const Position& b) const -> Position {
  return Position{.x = this->x / b.x, .y = this->y / b.y};
}

auto Position::operator%(const Position& b) const -> Position {
  return Position{.x = this->x % b.x, .y = this->y % b.y};
}

auto Size::fromWx(const wxSize& wx) -> Size {
  Size size;
  size.width = wx.GetWidth();
  size.height = wx.GetHeight();
  return size;
}

auto Size::operator==(const Size& rhs) const -> bool {
  return (this->width == rhs.width && this->height == rhs.height);
}

auto Size::operator!=(const Size& rhs) const -> bool { return !(*this == rhs); }

auto Size::operator-() const -> Size {
  return Size{.width = -this->width, .height = -this->height};
}

auto Size::operator+(const Size& b) const -> Size {
  return Size{.width = this->width + b.width,
              .height = this->height + b.height};
}

auto Size::operator-(const Size& b) const -> Size { return *this + -b; }

auto Size::operator*(const Size& b) const -> Size {
  return Size{.width = this->width * b.width,
              .height = this->height * b.height};
}

auto Size::operator*(double b) const -> Size {
  return Size{
      .width = static_cast<int64_t>(static_cast<double>(this->width) * b),
      .height = static_cast<int64_t>(static_cast<double>(this->height) * b)};
}

auto Size::operator/(const Size& b) const -> Size {
  return Size{.width = this->width / b.width,
              .height = this->height / b.height};
}

auto Size::operator%(const Size& b) const -> Size {
  return Size{.width = this->width % b.width,
              .height = this->height % b.height};
}

}  // namespace cszb_scoreboard
