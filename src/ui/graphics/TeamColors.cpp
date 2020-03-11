/*
ui/graphics/TeamColors.cpp: A singleton which holds the currently selected
colors for every team.

Copyright 2020 Tracy Beck

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

namespace cszb_scoreboard {

TeamColors* TeamColors::singleton_instance;

TeamColors* TeamColors::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new TeamColors();
  }
  return singleton_instance;
}

TeamColors::TeamColors() {
  // Initialize colors from configured defaults
  proto::ScreenSide home_side, away_side;
  home_side.set_home(true);
  away_side.set_away(true);

  setColor(home_side, TeamConfig::getInstance()->teamColor(home_side)[0]);
  setColor(away_side, TeamConfig::getInstance()->teamColor(away_side)[0]);
  all_color = Color("Black");
}

Color TeamColors::getColor(const proto::ScreenSide& side) {
  if (side.home() && side.away() && side.extra()) {
    return all_color;
  } else if (side.home()) {
    return home_color;
  } else if (side.away()) {
    return away_color;
  } else {
    wxLogDebug(
        "Attempting to get a color to a non-home, non-away, non-all side.");
    return Color("Black");
  }
}

void TeamColors::setColor(const proto::ScreenSide& side, Color color) {
  if (side.home() && side.away() && side.extra()) {
    all_color = color;
  } else if (side.home()) {
    home_color = color;
  } else if (side.away()) {
    away_color = color;
  } else {
    wxLogDebug("Attempting to save a color to a non-home, non-away side.");
  }
}

}  // namespace cszb_scoreboard
