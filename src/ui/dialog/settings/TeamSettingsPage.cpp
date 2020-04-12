/*
ui/dialog/settings/TeamSettingsPage.cpp: The page of the settings notebook which
handles team based settings.

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

#include "ui/dialog/settings/TeamSettingsPage.h"

#include "ui/UiUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

TeamSettingsPage::TeamSettingsPage(wxWindow* parent) : SettingsPage(parent) {
  wxSizer* sizer = UiUtil::sizer(0, 1);

  for (int i = 0; i < TeamConfig::getInstance()->numberOfTeams(); ++i) {
    TeamSettingsPanel* team_panel = new TeamSettingsPanel(this, i);
    team_settings_panels.push_back(team_panel);
    sizer->Add(team_panel, 0, wxALL, BORDER_SIZE);
  }

  SetSizerAndFit(sizer);
}

bool TeamSettingsPage::validateSettings() {
  // At present, nothing to validate
  return true;
}

void TeamSettingsPage::saveSettings() {
  for (int i = 0; i < team_settings_panels.size(); ++i) {
    TeamConfig::getInstance()->setColor(i,
                                        team_settings_panels[i]->teamColor());
  }
  TeamConfig::getInstance()->saveSettings();
}

}  // namespace cszb_scoreboard
