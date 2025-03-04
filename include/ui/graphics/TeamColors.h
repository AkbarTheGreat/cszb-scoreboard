/*
ui/graphics/TeamColors.h: A singleton which holds the currently selected colors
for every team.

Copyright 2020-2025 Tracy Beck

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

#include <ui/graphics/Color.h>  // for Color

#include "ScoreboardCommon.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {
namespace proto {
class ScreenSide;
}  // namespace proto

class TeamColors {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit TeamColors(SingletonClass c)
      : TeamColors(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  auto getColor(const proto::ScreenSide &side) -> Color;
  void setColor(const proto::ScreenSide &side, const Color &color);

  PUBLIC_TEST_ONLY
  TeamColors(SingletonClass c, Singleton *singleton);

 private:
  Color home_color;
  Color away_color;
  Color all_color;
};

}  // namespace cszb_scoreboard
