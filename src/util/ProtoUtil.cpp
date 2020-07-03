/*
util/ProtoUtil.cpp: Convenience methods for converting between proto values and
wxWidgets values.

Copyright 2019-2020 Tracy Beck

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

proto::Rectangle *ProtoUtil::protoRct(wxRect input, proto::Rectangle *output) {
  output->set_x(input.GetX());
  output->set_y(input.GetY());
  output->set_width(input.GetWidth());
  output->set_height(input.GetHeight());
  return output;
}

wxRect ProtoUtil::wxRct(proto::Rectangle input) {
  wxRect rect(input.x(), input.y(), input.width(), input.height());
  return rect;
}

proto::Color *ProtoUtil::protoClr(Color input, proto::Color *output) {
  output->set_rgb(input.GetRGB());
  return output;
}

Color ProtoUtil::wxClr(proto::Color input) { return Color(input.rgb()); }

wxFont ProtoUtil::wxScaledFont(proto::Font input, wxSize scale_size) {
  int scaled_size = scale_size.GetHeight() * input.size() / 75;

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

void ProtoUtil::setFontColor(proto::Font *font, Color color) {
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

proto::ScreenSide ProtoUtil::homeSide() {
  proto::ScreenSide side;
  side.set_home(true);
  return side;
}

proto::ScreenSide ProtoUtil::awaySide() {
  proto::ScreenSide side;
  side.set_away(true);
  return side;
}

proto::ScreenSide ProtoUtil::allSide() {
  proto::ScreenSide side;
  side.set_home(true);
  side.set_away(true);
  side.set_extra(true);
  return side;
}

proto::ScreenSide ProtoUtil::noSide() {
  proto::ScreenSide side;
  return side;
}

bool ProtoUtil::sideContains(proto::ScreenSide side,
                             proto::TeamInfo_TeamType team) {
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
