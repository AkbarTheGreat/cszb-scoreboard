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

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

TeamSelectionEntry::TeamSelectionEntry(swx::Panel* wx,
                                       TeamSelectionBox* owning_controller,
                                       const proto::TeamLibInfo& team)
    : Panel(wx) {
  parent = owning_controller;

  clear = button("Clear", true);
  home = radioButton();
  away = radioButton();
  name = label(team.name());
  default_team = label(TeamConfig::teamName(team.default_team_type()));
  // Add a little buffer on the right so the scrollbar doesn't cover text
  buffer = label("   ");

  positionWidgets();
  bindEvents();
}

void TeamSelectionEntry::positionWidgets() {
  int col = 0;
  addWidget(*clear, 0, col++);
  addWidget(*home, 0, col++);
  addWidget(*away, 0, col++);
  addWidget(*name, 0, col++);
  addWidget(*default_team, 0, col++);
  addWidget(*buffer, 0, col++);

  runSizer();
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
