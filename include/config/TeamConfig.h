/*
config/TeamConfig.h: This class is a configuration singleton which
represents the team specific configuration pieces.

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
#pragma once

#include <wx/wx.h>

#include <vector>

#include "config.pb.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

class TeamConfig {
 public:
  static TeamConfig *getInstance();
  std::vector<int> indicesForSide(proto::ScreenSide side);
  int numberOfTeams();
  void saveSettings();
  void setColor(int index, Color color);
  std::vector<proto::TeamInfo_TeamType> singleScreenOrder();
  proto::TeamInfo teamInfo(int index);
  Color teamColor(int index);
  std::vector<Color> teamColor(proto::ScreenSide side);
  wxString teamName(int index);

 private:
  static TeamConfig *singleton_instance;
  proto::TeamConfig team_config;
  TeamConfig();
  bool checkTeamConfig();
  bool checkTeamOrder();
  int indexForTeam(proto::TeamInfo_TeamType team);
  void setTeam(proto::TeamInfo *team, proto::TeamInfo_TeamType type);
};
}  // namespace cszb_scoreboard
