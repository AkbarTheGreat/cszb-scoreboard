/*
util/ProtoUtil.h: Convenience methods for converting between proto values and
wxWidgets values.

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

#include <wx/wx.h>
#include <wx/gdicmn.h>

#include "config.pb.h"
#include "ui/graphics/Color.h"
#include "wx/font.h"

namespace cszb_scoreboard {

class ProtoUtil {
 public:
  static auto protoRct(const wxRect &input, proto::Rectangle *output)
      -> proto::Rectangle *;
  static auto wxRct(const proto::Rectangle &input) -> wxRect;
  static auto protoClr(const Color &input, proto::Color *output)
      -> proto::Color *;
  static auto wxClr(const proto::Color &input) -> Color;
  static auto wxScaledFont(const proto::Font &input, const wxSize &scale_size)
      -> wxFont;
  static void setFontColor(proto::Font *font, const Color &color);
  static void defaultFont(proto::Font *font);
  static void validateFont(proto::Font *font);
  static auto homeSide() -> proto::ScreenSide;
  static auto awaySide() -> proto::ScreenSide;
  static auto allSide() -> proto::ScreenSide;
  static auto noSide() -> proto::ScreenSide;
  static auto teamSide(const proto::TeamInfo_TeamType &team)
      -> proto::ScreenSide;
  static auto sideContains(const proto::ScreenSide &side,
                           const proto::TeamInfo_TeamType &team) -> bool;
};

}  // namespace cszb_scoreboard
