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
#include "ui/widget/Panel.h"    // for Panel

namespace cszb_scoreboard {

TeamSelectionEntry::TeamSelectionEntry(Panel* panel,
                                       TeamSelectionBox* owning_controller,
                                       int32_t row,
                                       const proto::TeamLibInfo& team) {
  parent = owning_controller;
  this->panel = panel;
  index = row;

  clear = panel->button("Clear", true);
  home = panel->radioButton();
  away = panel->radioButton();
  name = panel->label(team.name());
  default_team = panel->label(TeamConfig::teamName(team.default_team_type()));

  positionWidgets();
  bindEvents();
}

void TeamSelectionEntry::positionWidgets() {
  int col = 0;
  int buff_num = 0;
  panel->addWidget(*clear, index, col++);
  panel->addWidget(*home, index, col++);
  panel->addWidget(*away, index, col++);
  panel->addWidget(*name, index, col++);
  panel->addWidget(*default_team, index, col++);
  panel->addSpacer({.width = 30, .height = 0}, index, col++);
}

void TeamSelectionEntry::bindEvents() {
  clear->bind(wxEVT_BUTTON, [this](wxCommandEvent& event) -> void {
    this->clearButtonPressed();
  });
}

void TeamSelectionEntry::clearButtonPressed() {
  home->setSelected(false);
  away->setSelected(false);
  teamSelectionChanged(proto::TeamInfo_TeamType_TEAM_ERROR);
}

void TeamSelectionEntry::teamSelectionChanged(
    proto::TeamInfo_TeamType new_team) {}

}  // namespace cszb_scoreboard
