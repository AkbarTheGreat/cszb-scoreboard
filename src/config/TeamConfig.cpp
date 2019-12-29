/*
config/TeamConfig.cpp: This class is a configuration singleton which
represents the team specific configuration pieces.

Copyright 2019 Tracy Beck

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

TeamConfig *TeamConfig::singleton_instance;

TeamConfig::TeamConfig() {
  team_config = Persistence::getInstance()->loadTeams();
  bool defaults_updated = false;

  // Since the team config is usually pretty small, establish defaults
  // one-by-one, if necessary.
  if (team_config.teams_size() < 1) {
    setTeam(team_config.add_teams(), proto::TeamInfo_TeamType_HOME_TEAM);
    defaults_updated = true;
  } else if (team_config.teams(0).team_type() !=
             proto::TeamInfo_TeamType_HOME_TEAM) {
    setTeam(team_config.mutable_teams(0), proto::TeamInfo_TeamType_HOME_TEAM);
    defaults_updated = true;
  }

  if (team_config.teams_size() < 2) {
    setTeam(team_config.add_teams(), proto::TeamInfo_TeamType_AWAY_TEAM);
    defaults_updated = true;
  } else if (team_config.teams(1).team_type() !=
             proto::TeamInfo_TeamType_AWAY_TEAM) {
    setTeam(team_config.mutable_teams(1), proto::TeamInfo_TeamType_AWAY_TEAM);
    defaults_updated = true;
  }
  if (defaults_updated) {
    saveSettings();
  }
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

TeamConfig *TeamConfig::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new TeamConfig();
  }
  return singleton_instance;
}

void TeamConfig::saveSettings() {
  Persistence::getInstance()->saveTeams(team_config);
}

}  // namespace cszb_scoreboard
