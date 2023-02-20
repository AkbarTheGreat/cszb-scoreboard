/*
ui/dialog/TeamLibraryDialog.cpp: A dialog which allows a user to select a team
from the team library or edit said library.

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

#include "ui/dialog/TeamLibraryDialog.h"

#include <vector>  // for vector

#include "config.pb.h"         // for TeamInfo_TeamType_AW...
#include "config/swx/defs.h"   // for wxID_CANCEL, wxID_OK
#include "config/swx/event.h"  // for wxEVT_BUTTON
// IWYU pragma: no_include <google/protobuf/repeated_ptr_field.h>

namespace cszb_scoreboard {
class ScoreControl;

namespace swx {
class PropertySheetDialog;
}  // namespace swx

constexpr int TEAMS_BEFORE_SCROLL = 5;

TeamLibraryDialog::TeamLibraryDialog(swx::PropertySheetDialog *wx,
                                     ScoreControl *parent, Singleton *singleton)
    : TabbedDialog(wx) {
  this->parent = parent;
  this->singleton = singleton;

  library =
      proto::TeamLibrary();  // TODO(akbar): Load/Save with Persistence.cpp

  // For testing, populate some default values.  These are just some teams we
  // use in Boston, they're placeholders.
  proto::TeamLibInfo *home_team_info = library.add_teams();
  home_team_info->set_default_team_type(proto::TeamInfo_TeamType_HOME_TEAM);
  home_team_info->set_name("Rozzie Square Pegs");
  home_team_info->set_image_path("c:\\logos\\RSP.png");
  home_team_info->set_is_relative(false);
  proto::TeamLibInfo *away_team_info = library.add_teams();
  away_team_info->set_default_team_type(proto::TeamInfo_TeamType_AWAY_TEAM);
  away_team_info->set_name("Boston Baked Beans");
  away_team_info->set_image_path("BBB.png");
  away_team_info->set_is_relative(true);
  proto::TeamLibInfo *third_team_info = library.add_teams();
  third_team_info->set_name("Waltham Sandwiches");
  proto::TeamLibInfo *fourth_team_info = library.add_teams();
  fourth_team_info->set_name("Brookline NSynchers");
  // End testing stuff to be deleted

  box_panel = panel();
  team_selection_scrolling = box_panel->scrollingPanel();
  team_selection = team_selection_scrolling->panel();
  bottom_panel = box_panel->panel();

  divider = bottom_panel->divider();

  name_entry = bottom_panel->text("");
  name_label = bottom_panel->label("Team name");

  file_name_label = bottom_panel->label("Logo Filename");
  file_name_entry = bottom_panel->text("");

  default_team_label = bottom_panel->label("Default Side");
  default_team_selector = bottom_panel->label("<PLACEHOLDER FOR DROPDOWN>");

  add_update_button = bottom_panel->button("Add");

  positionWidgets();
  bindEvents();
}

void TeamLibraryDialog::positionWidgets() {
  int row = -1;
  bottom_panel->addWidgetWithSpan(*divider, ++row, 0, 1, 2);

  bottom_panel->addWidget(*name_label, ++row, 0);
  bottom_panel->addWidget(*name_entry, row, 1);

  bottom_panel->addWidget(*file_name_label, ++row, 0);
  bottom_panel->addWidget(*file_name_entry, row, 1);

  bottom_panel->addWidget(*default_team_label, ++row, 0);
  bottom_panel->addWidget(*default_team_selector, row, 1);

  bottom_panel->addWidget(*add_update_button, ++row, 0);

  bottom_panel->runSizer();

  team_selection_scrolling->addWidget(*team_selection, 0, 0);

  populateTeamSelection();

  box_panel->addWidget(*team_selection_scrolling, 0, 0);
  box_panel->addWidget(*bottom_panel, 1, 0);

  box_panel->runSizer();

  addPage(*box_panel, "");
  runSizer();
  team_selection->runSizer();
  team_selection_scrolling->runSizer();
}

void TeamLibraryDialog::populateTeamSelection() {
  team_selection_entries.reserve(library.teams_size());
  for (const auto &team : library.teams()) {
    team_selection_entries.emplace_back(std::make_unique<TeamSelectionEntry>(
        team_selection->childPanel(), this, team));
  }

  int row = 0;
  for (const auto &entry : team_selection_entries) {
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
}

void TeamLibraryDialog::bindEvents() {
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onOk(); },
      wxID_OK);
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onCancel(); },
      wxID_CANCEL);
}

void TeamLibraryDialog::onOk() {
  if (validateSettings()) {
    saveSettings();
    close();
    return;
  }
}

void TeamLibraryDialog::onCancel() { close(); }

auto TeamLibraryDialog::validateSettings() -> bool { return true; }

void TeamLibraryDialog::saveSettings() {
  // TODO(akbar): Save
}

}  // namespace cszb_scoreboard
