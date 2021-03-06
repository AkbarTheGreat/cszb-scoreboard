/*
ui/dialog/settings/TeamSettingsPage.cpp: The page of the settings notebook which
handles team based settings.

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

#include "ui/dialog/settings/TeamSettingsPage.h"

#include "ui/UiUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

TeamSettingsPage::TeamSettingsPage(wxWindow* parent) : SettingsPage(parent) {
  wxSizer* sizer = UiUtil::sizer(0, 1);

  int i = 0;
  for (auto team : TeamConfig::getInstance()->singleScreenOrder()) {
    auto* team_panel = new TeamSettingsPanel(this, i++, team);
    team_settings_panels.push_back(team_panel);
    sizer->Add(team_panel, 0, wxALL, BORDER_SIZE);
  }

  SetSizerAndFit(sizer);
}

auto TeamSettingsPage::validateSettings() -> bool {
  // At present, nothing to validate
  return true;
}

void TeamSettingsPage::saveSettings() {
  bool restart_warning = false;
  std::vector<proto::TeamInfo_TeamType> team_order;
  std::vector<proto::TeamInfo_TeamType> previous_team_order =
      TeamConfig::getInstance()->singleScreenOrder();

  if (previous_team_order.size() != team_settings_panels.size()) {
    restart_warning = true;
  }

  for (auto* panel : team_settings_panels) {
    TeamConfig::getInstance()->setColor(panel->team(), panel->teamColor());

    if (restart_warning || previous_team_order.size() <= team_order.size() ||
        panel->team() != previous_team_order[team_order.size()]) {
      restart_warning = true;
    }

    team_order.push_back(panel->team());
  }

  if (restart_warning) {
    wxMessageBox(
        "WARNING: You have changed team ordering.  To see this take "
        "effect, you must restart the application.");
  }

  TeamConfig::getInstance()->setSingleScreenOrder(team_order);
  TeamConfig::getInstance()->saveSettings();
}

void TeamSettingsPage::swapTeams(int a, int b) {
  TeamSettingsPanel temp(this, 0, proto::TeamInfo_TeamType_TEAM_ERROR);
  temp.copyFrom(team_settings_panels[a]);
  team_settings_panels[a]->copyFrom(team_settings_panels[b]);
  team_settings_panels[b]->copyFrom(&temp);
}

}  // namespace cszb_scoreboard
