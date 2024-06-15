/*
ui/dialog/team_library/TeamSelectionBox.cpp: A box which lists all known teams
and information about them, with selections to chose home/away for the current
match.

Copyright 2023-2024 Tracy Beck

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
#include <cstdint>    // for int32_t, int64_t
#include <string>     // for string

#include "config.pb.h"                    // for TeamInfo_TeamType
#include "config/Persistence.h"           // for Persistence
#include "config/Position.h"              // for Size
#include "team_library.pb.h"              // for TeamLibInfo, TeamLibrary
#include "ui/dialog/TeamLibraryDialog.h"  // for TeamLibraryDialog
#include "ui/widget/ScrollingPanel.h"     // for ScrollingPanel
#include "ui/widget/Widget.h"             // for NO_BORDER
#include "util/FilesystemPath.h"          // for FilesystemPath
// IWYU pragma: no_include <google/protobuf/repeated_ptr_field.h>
// IWYU pragma: no_include "net/proto2/public/repeated_field.h"

namespace cszb_scoreboard {

constexpr int32_t TEAMS_BEFORE_SCROLL = 5;

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

void TeamSelectionBox::clearOldEntries() {
  team_selection->destroy();
  team_selection_entries.clear();
  team_selection = team_selection_scrolling->panel();
  team_selection_scrolling->addWidget(*team_selection, 0, 0, NO_BORDER);
}

void TeamSelectionBox::createEntries() {
  int32_t row = 0;
  if (!team_selection_entries.empty()) {
    clearOldEntries();
  }
  team_selection_entries.reserve(library.teams_size() + 1);
  team_selection_entries.emplace_back(std::make_unique<TeamSelectionEntry>(
      team_selection.get(), this, row++, placeholderTeam()));
  for (const auto& team : library.teams()) {
    team_selection_entries.emplace_back(std::make_unique<TeamSelectionEntry>(
        team_selection.get(), this, row++, team));

    if (row == TEAMS_BEFORE_SCROLL) {
      team_selection->runSizer();
      team_selection_scrolling->setMinSize(team_selection->size());
      team_selection_scrolling->runSizer();
    }
  }
  if (row < TEAMS_BEFORE_SCROLL) {
    team_selection->runSizer();
    team_selection_scrolling->setMinSize(team_selection->size());
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
  header_labels.push_back(header->label("Default"));
  header->addWidget(*header_labels.back(), 0, col,
                    TeamSelectionEntry::BORDER_SIZE);
  col += 2;

  // Delete button
  header_labels.push_back(header->label("Delete"));
  header->addWidget(*header_labels.back(), 0, col,
                    TeamSelectionEntry::BORDER_SIZE);
  col += 2;

  header->runSizer();

  for (int32_t entry_col = 0; entry_col < 4; entry_col++) {
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
  pad_text = "WWWWWWW";  // Wide enough for if we just have a -None- entry.
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

void TeamSelectionBox::teamSelectedForSide(int32_t row,
                                           proto::TeamInfo_TeamType team) {
  for (int32_t i = 0; i < team_selection_entries.size(); i++) {
    if (i != row) {
      team_selection_entries[i]->teamSelectionChanged(team);
    }
  }
}

void TeamSelectionBox::teamSelectedForEdit(int32_t row) {
  if (row == 0) {
    parent->clearEdit();
    return;
  }
  int32_t team_number = row - 1;
  auto team = library.teams(team_number);
  parent->editTeam(team_number, team.name(), FilesystemPath(team.image_path()),
                   team.default_team_type());
}

void TeamSelectionBox::addTeam(const std::string& name,
                               const FilesystemPath& logo,
                               proto::TeamInfo_TeamType type) {
  setTeamInfo(library.add_teams(), name, logo, type);
}

void TeamSelectionBox::changeTeam(int32_t team_number, const std::string& name,
                                  const FilesystemPath& logo,
                                  proto::TeamInfo_TeamType type) {
  setTeamInfo(library.mutable_teams(team_number), name, logo, type);
}

void TeamSelectionBox::deleteTeam(int32_t row_number) {
  int32_t team_number = row_number - 1;
  auto* teams = library.mutable_teams();
  teams->erase(teams->begin() + team_number);
  createEntries();
  updateList();
  parent->clearEdit();
}

void TeamSelectionBox::copyTeam(const proto::TeamLibInfo& from,
                                proto::TeamLibInfo* to) {
  to->set_name(from.name());
  to->set_image_path(from.image_path());
}

auto TeamSelectionBox::selectedTeams() -> proto::TeamLibraryDialogResponse {
  proto::TeamLibraryDialogResponse response;
  // Skip entry 0, as that's the "None" field.  Lack of assignment is how we
  // communicate that the team is unset.
  for (int i = 1; i < team_selection_entries.size(); i++) {
    if (team_selection_entries[i]->isHome()) {
      copyTeam(library.teams(i - 1), response.mutable_home());
    }
    if (team_selection_entries[i]->isAway()) {
      copyTeam(library.teams(i - 1), response.mutable_away());
    }
  }
  return response;
}
void TeamSelectionBox::saveLibrary() {
  singleton->persistence()->saveTeamLibrary(library);
}

// If type is a valid team type, clear out all entries in the library with
// that type.
void TeamSelectionBox::removeType(proto::TeamInfo_TeamType type) {
  if (type == proto::TeamInfo_TeamType_TEAM_ERROR) {
    return;
  }
  for (auto& team : *library.mutable_teams()) {
    if (team.default_team_type() == type) {
      team.set_default_team_type(proto::TeamInfo_TeamType_TEAM_ERROR);
    }
  }
}

void TeamSelectionBox::setTeamInfo(proto::TeamLibInfo* team,
                                   const std::string& name,
                                   const FilesystemPath& logo,
                                   proto::TeamInfo_TeamType type) {
  // Remove this team type as a default in the library, so it can be set for
  // this team.
  removeType(type);
  team->set_name(name);
  team->set_image_path(logo.string());
  team->set_default_team_type(type);
  createEntries();
  updateList();
}

}  // namespace cszb_scoreboard
