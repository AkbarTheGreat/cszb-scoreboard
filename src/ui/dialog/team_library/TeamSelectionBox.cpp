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

#include <algorithm>  // for max
#include <cstdint>    // for int64_t, int32_t

#include "config/Persistence.h"        // for Persistence
#include "config/Position.h"           // for Size
#include "team_library.pb.h"           // for TeamLibrary
#include "ui/widget/ScrollingPanel.h"  // for ScrollingPanel
#include "ui/widget/Widget.h"          // for NO_BORDER
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
  team_selection_scrolling->addWidget(*team_selection, 0, 0, NO_BORDER);
  addWidget(*team_selection_scrolling, 1, 0, NO_BORDER);

  createEntries();
  createHeader();
  addWidget(*header, 0, 0, NO_BORDER);

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
  int32_t col = 0;

  // Empty space for the clear button.
  header->addSpacer({.width = 1}, 0, col, TeamSelectionEntry::BORDER_SIZE);
  col += 2;

  // Team selectors
  header_labels.push_back(header->label("Home"));
  header->addWidget(*header_labels.back(), 0, col,
                    TeamSelectionEntry::BORDER_SIZE);
  col += 2;
  header_labels.push_back(header->label("Away"));
  header->addWidget(*header_labels.back(), 0, col,
                    TeamSelectionEntry::BORDER_SIZE);
  col += 2;

  // Team name
  header_labels.push_back(header->label("Team Name"));
  header->addWidget(*header_labels.back(), 0, col,
                    TeamSelectionEntry::BORDER_SIZE);
  col += 2;

  // Team type
  header_labels.push_back(header->label("Team Type"));
  header->addWidget(*header_labels.back(), 0, col,
                    TeamSelectionEntry::BORDER_SIZE);
  col += 2;

  header->runSizer();

  for (int entry_col = 0; entry_col < 4; entry_col++) {
    int64_t width = team_selection->sizeOfWidgetAtLocation(0, entry_col).width -
                    header->sizeOfWidgetAtLocation(0, entry_col * 2).width;
    if (width < 0) {
      width = 0;
    }
    header->addSpacer({.width = width}, 0, entry_col * 2 + 1, NO_BORDER);
  }
  header->runSizer();
}

// Helper method for createHeader
auto TeamSelectionBox::maxRowLength(int64_t column) -> int64_t {
  int64_t max_len = 0;
  for (int row = 0; row < team_selection_entries.size(); row++) {
    max_len = std::max(max_len,
                       team_selection->sizeOfWidgetAtLocation(0, column).width);
  }
  return max_len;
}

void TeamSelectionBox::bindEvents() {}

void TeamSelectionBox::updateList() {
  team_selection->runSizer();
  team_selection_scrolling->runSizer();
}

}  // namespace cszb_scoreboard
