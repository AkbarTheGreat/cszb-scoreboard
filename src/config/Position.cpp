/*
config/Position.cpp: A few structs which help with passing around positions or
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

#include "config/Position.h"

namespace cszb_scoreboard {

auto Position::fromWx(const wxPoint& wx) -> Position {
  Position pos;
  pos.x = wx.x;
  pos.y = wx.y;
  return pos;
}

auto Size::fromWx(const wxSize& wx) -> Size {
  Size size;
  size.width = wx.GetWidth();
  size.height = wx.GetHeight();
  return size;
}

}  // namespace cszb_scoreboard
