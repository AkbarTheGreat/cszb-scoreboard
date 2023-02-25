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

#include <cstdint>  // for int32_t, int64_t
#include <string>   // for string

#include "config/Persistence.h"        // for Persistence
#include "config/Position.h"           // for Size
#include "team_library.pb.h"           // for TeamLibInfo, TeamLibrary
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
  team_selection_entries.reserve(library.teams_size() + 1);
  team_selection_entries.emplace_back(std::make_unique<TeamSelectionEntry>(
      team_selection.get(), this, row++, placeholderTeam()));
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
auto TeamSelectionBox::placeholderTeam() -> proto::TeamLibInfo {
  proto::TeamLibInfo placeholder_team;
  // We use 'W' for our placeholder name because it tends to be the widest
  // in variable-width fonts.
  pad_text = "WWWWWWW"; // Wide enough for if we just have a -None- entry.
  for (const auto& team : library.teams()) {
    if (team.name().length() > pad_text.length()) {
      pad_text.resize(team.name().length(), 'W');
    }
  }
  placeholder_team.set_name(pad_text);
  return placeholder_team;
}

void TeamSelectionBox::bindEvents() {}

void TeamSelectionBox::updateList() {
  team_selection_entries[0]->setName(pad_text);
  team_selection->runSizer();
  team_selection_scrolling->runSizer();
  team_selection_entries[0]->setName(" -None-");
}

void TeamSelectionBox::teamSelected(int32_t row,
                                    proto::TeamInfo_TeamType team) {
  for (int i = 0; i < team_selection_entries.size(); i++) {
    if (i != row) {
      team_selection_entries[i]->teamSelectionChanged(team);
    }
  }
}

}  // namespace cszb_scoreboard
