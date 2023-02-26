/*
ui/dialog/team_library/TeamSelectionEntry.cpp: A single entry in the team
selection box with all of the pertinent information for a given team. from the
team library or edit said library.

Copyright 2023 Tracy Beck

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

#include "ui/dialog/team_library/TeamSelectionEntry.h"

#include <wx/event.h>  // for wxCommandEvent (ptr only), wxEVT_BUTTON

#include "config/TeamConfig.h"  // for TeamConfig
#include "team_library.pb.h"    // for TeamLibInfo
#include "ui/dialog/team_library/TeamSelectionBox.h"
#include "ui/widget/Panel.h"  // for Panel

namespace cszb_scoreboard {

constexpr int32_t SCROLLBAR_BUFFER_WIDTH_PIXELS = 20;
constexpr int32_t TEAM_SELECTION_SIZE_PIXELS = 50;

TeamSelectionEntry::TeamSelectionEntry(Panel* panel,
                                       TeamSelectionBox* owning_controller,
                                       int32_t row,
                                       const proto::TeamLibInfo& team) {
  parent = owning_controller;
  this->panel = panel;
  index = row;

  home = panel->radioButton();
  away = panel->radioButton();
  name = panel->label(team.name());
  default_team = panel->label(TeamConfig::teamName(team.default_team_type()));

  positionWidgets();
  bindEvents();

  if (index == 0) {
    home->setSelected(true);
    away->setSelected(true);
  }
}

void TeamSelectionEntry::hide() {
  home->hide();
  away->hide();
  name->hide();
  default_team->hide();
}

void TeamSelectionEntry::show() {
  home->show();
  away->show();
  name->show();
  default_team->show();
}

void TeamSelectionEntry::setName(const std::string& name) {
  this->name->set(name);
}

void TeamSelectionEntry::positionWidgets() {
  int col = 0;
  int buff_num = 0;
  panel->addWidget(*home, index, col++, BORDER_SIZE);
  home->setMinSize({.width = TEAM_SELECTION_SIZE_PIXELS});
  panel->addWidget(*away, index, col++, BORDER_SIZE);
  away->setMinSize({.width = TEAM_SELECTION_SIZE_PIXELS});
  panel->addWidget(*name, index, col++, BORDER_SIZE);
  panel->addWidget(*default_team, index, col++, BORDER_SIZE);
  panel->addSpacer({.width = SCROLLBAR_BUFFER_WIDTH_PIXELS}, index, col++,
                   BORDER_SIZE);
}

void TeamSelectionEntry::bindEvents() {
  home->bind(wxEVT_RADIOBUTTON,
             [this](wxCommandEvent& event) -> void { homeButtonPressed(); });
  away->bind(wxEVT_RADIOBUTTON,
             [this](wxCommandEvent& event) -> void { awayButtonPressed(); });
}

void TeamSelectionEntry::homeButtonPressed() {
  parent->teamSelected(index, proto::TeamInfo_TeamType_HOME_TEAM);
}

void TeamSelectionEntry::awayButtonPressed() {
  parent->teamSelected(index, proto::TeamInfo_TeamType_AWAY_TEAM);
}

void TeamSelectionEntry::teamSelectionChanged(proto::TeamInfo_TeamType team) {
  if (team == proto::TeamInfo_TeamType_HOME_TEAM) {
    home->setSelected(false);
  }
  if (team == proto::TeamInfo_TeamType_AWAY_TEAM) {
    away->setSelected(false);
  }
}

}  // namespace cszb_scoreboard
