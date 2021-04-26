/*
ui/graphics/TeamColors.cpp: A singleton which holds the currently selected
colors for every team.

Copyright 2020-2021 Tracy Beck

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

#include "ui/graphics/TeamColors.h"

#include "config/TeamConfig.h"
#include "util/Log.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

auto TeamColors::getInstance() -> TeamColors* {
  static TeamColors singleton;
  return &singleton;
}

TeamColors::TeamColors() {
  // Initialize colors from configured defaults
  setColor(ProtoUtil::homeSide(),
           TeamConfig::getInstance()->teamColor(ProtoUtil::homeSide())[0]);
  setColor(ProtoUtil::awaySide(),
           TeamConfig::getInstance()->teamColor(ProtoUtil::awaySide())[0]);
  all_color = Color("Black");
}

auto TeamColors::getColor(const proto::ScreenSide& side) -> Color {
  if (side.home() && side.away() && side.extra()) {
    return all_color;
  }
  if (side.home()) {
    return home_color;
  }
  if (side.away()) {
    return away_color;
  }
  LogDebug("Attempting to get a color to a non-home, non-away, non-all side.");
  return Color("Black");
}

void TeamColors::setColor(const proto::ScreenSide& side, const Color& color) {
  if (side.home() && side.away() && side.extra()) {
    all_color = color;
  } else if (side.home()) {
    home_color = color;
  } else if (side.away()) {
    away_color = color;
  } else {
    LogDebug("Attempting to save a color to a non-home, non-away side.");
  }
}

}  // namespace cszb_scoreboard
