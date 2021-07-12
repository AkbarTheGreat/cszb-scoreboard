/*
config/TeamConfig.h: This class is a configuration singleton which
represents the team specific configuration pieces.

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
#include <wx/string.h>
#include <vector>

#include "config.pb.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

class TeamConfig {
 public:
  static auto getInstance() -> TeamConfig *;
  auto numberOfTeams() -> int;
  void saveSettings();
  void setColor(proto::TeamInfo_TeamType team, const Color &color);
  void setSingleScreenOrder(const std::vector<proto::TeamInfo_TeamType> &order);
  auto singleScreenOrder() -> std::vector<proto::TeamInfo_TeamType>;
  auto teamColor(proto::TeamInfo_TeamType team) -> Color;
  auto teamColor(const proto::ScreenSide &side) -> std::vector<Color>;
  auto teamName(proto::TeamInfo_TeamType team) -> wxString;

 private:
  proto::TeamConfig team_config;
  static void setTeam(proto::TeamInfo *team, proto::TeamInfo_TeamType type);
  static auto teamsForSide(const proto::ScreenSide &side)
      -> std::vector<proto::TeamInfo_TeamType>;
  TeamConfig();
  auto checkTeamConfig() -> bool;
  auto checkTeamOrder() -> bool;
  auto indexForTeam(proto::TeamInfo_TeamType team) -> int;
};
}  // namespace cszb_scoreboard
