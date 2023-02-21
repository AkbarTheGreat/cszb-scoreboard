/*
ui/dialog/team_library/TeamSelectionBox.cpp: A box which lists all known teams
and information about them, with selections to chose home/away for the current
match.

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

#include "ui/dialog/team_library/TeamSelectionBox.h"

#include <google/protobuf/repeated_ptr_field.h>  // for RepeatedPtrField

#include "config/Persistence.h"        // for Persistence
#include "team_library.pb.h"           // for TeamLibrary
#include "ui/widget/ScrollingPanel.h"  // for ScrollingPanel

namespace cszb_scoreboard {

constexpr int TEAMS_BEFORE_SCROLL = 5;

namespace swx {
class Panel;
}  // namespace swx

TeamSelectionBox::TeamSelectionBox(swx::Panel* wx,
                                   TeamLibraryDialog* owning_controller,
                                   Singleton* singleton)
    : Panel(wx) {
  parent = owning_controller;
  this->singleton = singleton;

  team_selection_scrolling = scrollingPanel();
  team_selection = team_selection_scrolling->panel();

  positionWidgets();
  bindEvents();
}

void TeamSelectionBox::positionWidgets() {
  auto library = singleton->persistence()->loadTeamLibrary();
  team_selection_scrolling->addWidget(*team_selection, 0, 0);
  addWidget(*team_selection_scrolling, 0, 0);

  team_selection_entries.reserve(library.teams_size());
  for (const auto& team : library.teams()) {
    team_selection_entries.emplace_back(std::make_unique<TeamSelectionEntry>(
        team_selection->childPanel(), this, team));
  }

  int row = 0;
  for (const auto& entry : team_selection_entries) {
    team_selection->addWidget(*entry, row++, 0);
    if (row == TEAMS_BEFORE_SCROLL) {
      team_selection->runSizer();
      team_selection_scrolling->runSizer();
    }
  }
  if (row < TEAMS_BEFORE_SCROLL) {
    team_selection->runSizer();
    team_selection_scrolling->runSizer();
  }

  runSizer();
}

void TeamSelectionBox::bindEvents() {}

void TeamSelectionBox::updateList() {
  team_selection->runSizer();
  team_selection_scrolling->runSizer();
}

}  // namespace cszb_scoreboard
