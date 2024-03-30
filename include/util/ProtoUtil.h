/*
util/ProtoUtil.h: Convenience methods for converting between proto values and
wxWidgets values.

Copyright 2019-2023 Tracy Beck

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

#include <wx/gdicmn.h>  // for wxRect

#include "config.pb.h"          // for ScreenSide, Font (ptr only), Color (p...
#include "ui/graphics/Color.h"  // for Color
#include "wx/font.h"            // for wxFont

namespace cszb_scoreboard {
struct Size;

class ProtoUtil {
 public:
  static auto protoRct(const wxRect &input, proto::Rectangle *output)
      -> proto::Rectangle *;
  static auto wxRct(const proto::Rectangle &input) -> wxRect;
  static auto protoClr(const Color &input, proto::Color *output)
      -> proto::Color *;
  static auto wxClr(const proto::Color &input) -> Color;
  static auto wxScaledFont(const proto::Font &input, const Size &scale_size)
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
