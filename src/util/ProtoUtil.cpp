/*
util/ProtoUtil.cpp: Convenience methods for converting between proto values and
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

#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const float DEFAULT_FONT_SIZE = 10;
const int SCALE_FACTOR = 75;

auto ProtoUtil::protoRct(const wxRect &input, proto::Rectangle *output)
    -> proto::Rectangle * {
  output->set_x(input.GetX());
  output->set_y(input.GetY());
  output->set_width(input.GetWidth());
  output->set_height(input.GetHeight());
  return output;
}

auto ProtoUtil::wxRct(const proto::Rectangle &input) -> wxRect {
  wxRect rect(input.x(), input.y(), input.width(), input.height());
  return rect;
}

auto ProtoUtil::protoClr(const Color &input, proto::Color *output)
    -> proto::Color * {
  output->set_rgb(input.GetRGB());
  return output;
}

auto ProtoUtil::wxClr(const proto::Color &input) -> Color {
  return Color(input.rgb());
}

auto ProtoUtil::wxScaledFont(const proto::Font &input, const wxSize &scale_size)
    -> wxFont {
  int scaled_size = scale_size.GetHeight() * input.size() / SCALE_FACTOR;

  wxFontInfo font_info(scaled_size);

  switch (input.style()) {
    case proto::Font_FontStyle_FONT_STYLE_IMPACT:
      font_info.FaceName("Impact");
      break;
    case proto::Font_FontStyle_FONT_STYLE_UNDEFINED:
    default:
      // Pick a reasonable default.
      font_info.Family(wxFONTFAMILY_MODERN);
      break;
  }

  return font_info.AntiAliased();
}

void ProtoUtil::setFontColor(proto::Font *font, const Color &color) {
  protoClr(color, font->mutable_color());
}

void ProtoUtil::defaultFont(proto::Font *font) {
  font->set_size(DEFAULT_FONT_SIZE);
  validateFont(font);
}

void ProtoUtil::validateFont(proto::Font *font) {
  if (font->style() == proto::Font_FontStyle_FONT_STYLE_UNDEFINED) {
#ifdef WIN32
    font->set_style(proto::Font_FontStyle_FONT_STYLE_IMPACT);
#else
    font->set_style(proto::Font_FontStyle_FONT_STYLE_MODERN);
#endif
  }
}

auto ProtoUtil::homeSide() -> proto::ScreenSide {
  proto::ScreenSide side;
  side.set_home(true);
  return side;
}

auto ProtoUtil::awaySide() -> proto::ScreenSide {
  proto::ScreenSide side;
  side.set_away(true);
  return side;
}

auto ProtoUtil::allSide() -> proto::ScreenSide {
  proto::ScreenSide side;
  side.set_home(true);
  side.set_away(true);
  side.set_extra(true);
  return side;
}

auto ProtoUtil::noSide() -> proto::ScreenSide {
  proto::ScreenSide side;
  return side;
}

auto ProtoUtil::teamSide(const proto::TeamInfo_TeamType &team)
    -> proto::ScreenSide {
  switch (team) {
    case proto::TeamInfo_TeamType_HOME_TEAM:
      return homeSide();
    case proto::TeamInfo_TeamType_AWAY_TEAM:
      return awaySide();
    case proto::TeamInfo_TeamType_EXTRA_TEAM:  // Extra isn't implemented yet
    case proto::TeamInfo_TeamType_TEAM_ERROR:
    default:
      return noSide();
  }
}

auto ProtoUtil::sideContains(const proto::ScreenSide &side,
                             const proto::TeamInfo_TeamType &team) -> bool {
  switch (team) {
    case proto::TeamInfo_TeamType_HOME_TEAM:
      return side.home();
    case proto::TeamInfo_TeamType_AWAY_TEAM:
      return side.away();
    case proto::TeamInfo_TeamType_EXTRA_TEAM:
      return side.extra();
    case proto::TeamInfo_TeamType_TEAM_ERROR:
      return side.error();
    default:
      return false;
  }
}

}  // namespace cszb_scoreboard
