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

#include <algorithm>
#include <cstdint>  // for int32_t

#include "config/Persistence.h"        // for Persistence
#include "team_library.pb.h"           // for TeamLibrary
#include "ui/widget/ScrollingPanel.h"  // for ScrollingPanel
// IWYU pragma: no_include <google/protobuf/repeated_ptr_field.h>

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

  library = singleton->persistence()->loadTeamLibrary();

  team_selection_scrolling = scrollingPanel();
  team_selection = team_selection_scrolling->panel();

  positionWidgets();
  bindEvents();
}

void TeamSelectionBox::positionWidgets() {
  team_selection_scrolling->addWidget(*team_selection, 0, 0);
  addWidget(*team_selection_scrolling, 1, 0);

  createEntries();
  createHeader();
  addWidget(*header, 0, 0);

  runSizer();
}

void TeamSelectionBox::createEntries() {
  int32_t row = 0;
  team_selection_entries.reserve(library.teams_size());
  for (const auto& team : library.teams()) {
    team_selection_entries.emplace_back(std::make_unique<TeamSelectionEntry>(
        team_selection.get(), this, row++, team));

    if (row == TEAMS_BEFORE_SCROLL) {
      team_selection->runSizer();
      team_selection_scrolling->runSizer();
    }
  }
  if (row < TEAMS_BEFORE_SCROLL) {
    team_selection->runSizer();
    team_selection_scrolling->runSizer();
  }
}

void TeamSelectionBox::createHeader() {
  header = panel();

  // Clear button
  header_labels.push_back(header->label(""));

  // Team selectors
  header_labels.push_back(header->label("Home"));
  header_labels.push_back(header->label("Away"));

  // Team name
  header_labels.push_back(header->label("Team Name"));

  // Team type
  header_labels.push_back(header->label("Team Type"));

  int32_t col = 0;
  for (const auto& label : header_labels) {
    header->addWidget(*label, 0, col++);
  }
  header->runSizer();
}

void TeamSelectionBox::bindEvents() {}

void TeamSelectionBox::updateList() {
  team_selection->runSizer();
  team_selection_scrolling->runSizer();
}

}  // namespace cszb_scoreboard
