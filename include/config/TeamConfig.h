/*
config/TeamConfig.h: This class is a configuration singleton which
represents the team specific configuration pieces.

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

#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"
#include "config.pb.h"          // for TeamInfo_TeamType, ScreenSide (ptr only)
#include "ui/graphics/Color.h"  // for Color
#include "util/Singleton.h"

namespace cszb_scoreboard {

class TeamConfig {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit TeamConfig(SingletonClass c)
      : TeamConfig(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  auto numberOfTeams() -> int;
  void saveSettings();
  void setColor(proto::TeamInfo_TeamType team, const Color &color);
  void setSingleScreenOrder(const std::vector<proto::TeamInfo_TeamType> &order);
  auto singleScreenOrder() -> std::vector<proto::TeamInfo_TeamType>;
  auto teamColor(proto::TeamInfo_TeamType team) -> Color;
  auto teamColor(const proto::ScreenSide &side) -> std::vector<Color>;
  static auto teamName(proto::TeamInfo_TeamType team) -> std::string;

  PUBLIC_TEST_ONLY
  TeamConfig(SingletonClass c, Singleton *singleton);

 private:
  proto::TeamConfig team_config;
  Singleton *singleton;
  static void setTeam(proto::TeamInfo *team, proto::TeamInfo_TeamType type);
  static auto teamsForSide(const proto::ScreenSide &side)
      -> std::vector<proto::TeamInfo_TeamType>;
  auto checkTeamConfig() -> bool;
  auto checkTeamOrder() -> bool;
  auto indexForTeam(proto::TeamInfo_TeamType team) -> int;
};
}  // namespace cszb_scoreboard
