/*
config/TeamConfig.cpp: This class is a configuration singleton which
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

#include "config/TeamConfig.h"

#include "config/Persistence.h"
#include "util/ProtoUtil.h"
namespace cszb_scoreboard {

TeamConfig *TeamConfig::singleton_instance = nullptr;

TeamConfig *TeamConfig::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new TeamConfig();
  }
  return singleton_instance;
}

TeamConfig::TeamConfig() {
  team_config = Persistence::getInstance()->loadTeams();
  bool settings_changed = false;

  settings_changed = checkTeamConfig() || settings_changed;
  settings_changed = checkTeamOrder() || settings_changed;

  if (settings_changed) {
    saveSettings();
  }
}

bool TeamConfig::checkTeamConfig() {
  bool teams_updated = false;

  // Since the team config is usually pretty small, establish defaults
  // one-by-one, if necessary.
  if (team_config.teams_size() < 1) {
    setTeam(team_config.add_teams(), proto::TeamInfo_TeamType_HOME_TEAM);
    teams_updated = true;
  } else if (team_config.teams(0).team_type() !=
             proto::TeamInfo_TeamType_HOME_TEAM) {
    setTeam(team_config.mutable_teams(0), proto::TeamInfo_TeamType_HOME_TEAM);
    teams_updated = true;
  }

  if (team_config.teams_size() < 2) {
    setTeam(team_config.add_teams(), proto::TeamInfo_TeamType_AWAY_TEAM);
    teams_updated = true;
  } else if (team_config.teams(1).team_type() !=
             proto::TeamInfo_TeamType_AWAY_TEAM) {
    setTeam(team_config.mutable_teams(1), proto::TeamInfo_TeamType_AWAY_TEAM);
    teams_updated = true;
  }

  // TODO: Add support for "Extra" team

  return teams_updated;
}

bool TeamConfig::checkTeamOrder() {
  bool order_updated = false;

  // TODO: Check for and remove duplicates.

  if (team_config.single_screen_order_size() < 2) {
    bool home_present = false;
    bool away_present = false;
    for (int i = 0; i < team_config.single_screen_order_size(); i++) {
      if (team_config.single_screen_order()[i] ==
          proto::TeamInfo_TeamType_HOME_TEAM) {
        home_present = true;
      }
      if (team_config.single_screen_order()[i] ==
          proto::TeamInfo_TeamType_AWAY_TEAM) {
        away_present = true;
      }
    }
    if (!home_present) {
      team_config.add_single_screen_order(proto::TeamInfo_TeamType_HOME_TEAM);
      order_updated = true;
    }
    if (!away_present) {
      team_config.add_single_screen_order(proto::TeamInfo_TeamType_AWAY_TEAM);
      order_updated = true;
    }
  }

  // TODO: Add support for "Extra" team;
  return order_updated;
}

void TeamConfig::setTeam(proto::TeamInfo *team, proto::TeamInfo_TeamType type) {
  team->set_team_type(type);
  proto::Color *color = team->mutable_team_color();
  switch (type) {
    case proto::TeamInfo_TeamType_HOME_TEAM:
      ProtoUtil::protoClr(Color("Blue"), color);
      break;
    case proto::TeamInfo_TeamType_AWAY_TEAM:
      ProtoUtil::protoClr(Color("Red"), color);
      break;
    default:
      ProtoUtil::protoClr(Color("Green"), color);
      break;
  }
}

std::vector<proto::TeamInfo_TeamType> TeamConfig::singleScreenOrder() {
  std::vector<proto::TeamInfo_TeamType> order;
  for (auto config_entry : team_config.single_screen_order()) {
    order.push_back((proto::TeamInfo_TeamType)config_entry);
  }
  return order;
}

std::vector<int> TeamConfig::indicesForSide(proto::ScreenSide side) {
  std::vector<int> index_list;
  if (side.home()) {
    index_list.push_back(indexForTeam(proto::TeamInfo_TeamType_HOME_TEAM));
  }
  if (side.away()) {
    index_list.push_back(indexForTeam(proto::TeamInfo_TeamType_AWAY_TEAM));
  }
  if (side.extra()) {
    index_list.push_back(indexForTeam(proto::TeamInfo_TeamType_EXTRA_TEAM));
  }
  return index_list;
}

int TeamConfig::indexForTeam(proto::TeamInfo_TeamType team) {
  for (int i = 0; i < team_config.teams_size(); ++i) {
    if (team_config.teams(i).team_type() == team) {
      return i;
    }
  }
  return 0;  // Not a great default, but we can live with it.
}

Color TeamConfig::teamColor(int index) {
  return ProtoUtil::wxClr(team_config.teams(index).team_color());
}

std::vector<Color> TeamConfig::teamColor(proto::ScreenSide side) {
  std::vector<int> indices = indicesForSide(side);
  std::vector<Color> colors;
  for (auto index : indices) {
    colors.push_back(teamColor(index));
  }
  return colors;
}

wxString TeamConfig::teamName(int index) {
  switch (team_config.teams(index).team_type()) {
    case proto::TeamInfo_TeamType_HOME_TEAM:
      return wxT("Home");
    case proto::TeamInfo_TeamType_AWAY_TEAM:
      return wxT("Away");
    default:
      return wxT("Unknown");
  }
}

int TeamConfig::numberOfTeams() { return team_config.teams_size(); }

void TeamConfig::setColor(int index, Color color) {
  proto::TeamInfo *team_info = team_config.mutable_teams(index);
  proto::Color *proto_color = team_info->mutable_team_color();
  ProtoUtil::protoClr(color, proto_color);
}

proto::TeamInfo TeamConfig::teamInfo(int index) {
  assert(index < team_config.teams_size() && index >= 0);
  return team_config.teams(index);
}

void TeamConfig::saveSettings() {
  Persistence::getInstance()->saveTeams(team_config);
}

}  // namespace cszb_scoreboard
