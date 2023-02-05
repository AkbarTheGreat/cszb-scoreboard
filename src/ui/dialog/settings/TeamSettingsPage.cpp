/*
ui/dialog/settings/TeamSettingsPage.cpp: The page of the settings notebook which
handles team based settings.

Copyright 2019-2023 Tracy Beck

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

#include "ui/dialog/settings/TeamSettingsPage.h"

#include <utility>  // for move

#include "ScoreboardCommon.h"   // for DEFAULT_BORDER_SIZE
#include "config.pb.h"          // for TeamInfo_TeamType, TeamInfo_TeamType_...
#include "config/TeamConfig.h"  // for TeamConfig
#include "ui/widget/PopUp.h"

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

TeamSettingsPage::TeamSettingsPage(swx::Panel *wx, Singleton *singleton)
    : SettingsPage(wx) {
  this->singleton = singleton;
  int i = 0;
  for (auto team : singleton->teamConfig()->singleScreenOrder()) {
    auto team_panel =
        std::make_unique<TeamSettingsPanel>(childPanel(), i++, team, this);
    addWidget(*team_panel, i - 1, 0);
    team_settings_panels.push_back(std::move(team_panel));
  }

  runSizer();
}

auto TeamSettingsPage::validateSettings() -> bool {
  // At present, nothing to validate
  return true;
}

void TeamSettingsPage::saveSettings() {
  bool restart_warning = false;
  std::vector<proto::TeamInfo_TeamType> team_order;
  std::vector<proto::TeamInfo_TeamType> previous_team_order =
      singleton->teamConfig()->singleScreenOrder();

  if (previous_team_order.size() != team_settings_panels.size()) {
    restart_warning = true;
  }

  for (const auto &panel : team_settings_panels) {
    singleton->teamConfig()->setColor(panel->team(), panel->teamColor());

    if (restart_warning || previous_team_order.size() <= team_order.size() ||
        panel->team() != previous_team_order[team_order.size()]) {
      restart_warning = true;
    }

    team_order.push_back(panel->team());
  }

  if (restart_warning) {
    PopUp::Message(
        "WARNING: You have changed team ordering.  To see this take "
        "effect, you must restart the application.");
  }

  singleton->teamConfig()->setSingleScreenOrder(team_order);
  singleton->teamConfig()->saveSettings();
}

void TeamSettingsPage::swapTeams(int a, int b) {
  TeamSettingsPanel temp(childPanel(), 0, proto::TeamInfo_TeamType_TEAM_ERROR,
                         this);
  temp.copyFrom(*team_settings_panels[a]);
  team_settings_panels[a]->copyFrom(*team_settings_panels[b]);
  team_settings_panels[b]->copyFrom(temp);
}

}  // namespace cszb_scoreboard
