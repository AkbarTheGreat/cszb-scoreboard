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

#include <string>  // for string, operator==
#include <vector>  // for vector

#include "config/swx/defs.h"                    // for wxID_CANCEL, wxID_OK
#include "config/swx/event.h"                   // for wxEVT_BUTTON, wxEVT_C...
#include "ui/component/control/ScoreControl.h"  // for ScoreControl
#include "ui/widget/DropDown.h"                 // for DropDown
#include "util/FilesystemPath.h"                // for FilesystemPath
// IWYU pragma: no_include <google/protobuf/repeated_ptr_field.h>

namespace cszb_scoreboard {

namespace swx {
class PropertySheetDialog;
}  // namespace swx

TeamLibraryDialog::TeamLibraryDialog(swx::PropertySheetDialog *wx,
                                     ScoreControl *parent, Singleton *singleton)
    : TabbedDialog(wx) {
  this->parent = parent;
  this->singleton = singleton;
  row_for_edit = -1;

  box_panel = panel();
  team_selection =
      std::make_unique<TeamSelectionBox>(box_panel->childPanel(), this);
  bottom_panel = box_panel->panel();

  divider = bottom_panel->divider();

  name_entry = bottom_panel->text("");
  name_label = bottom_panel->label("Team Name");

  file_name_label = bottom_panel->label("Logo Filename");
  file_name_entry = bottom_panel->text("");

  default_team_label = bottom_panel->label("Default Side");
  std::vector<std::string> choices = {"", "Home", "Away"};
  default_team_selector = bottom_panel->dropDown(choices);

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

  box_panel->addWidget(*team_selection, 0, 0);
  box_panel->addWidget(*bottom_panel, 1, 0);

  addPage(*box_panel, "");
  runSizer();
  team_selection->updateList();
}

void TeamLibraryDialog::bindEvents() {
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onOk(); },
      wxID_OK);
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onCancel(); },
      wxID_CANCEL);
  add_update_button->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->onAddOrUpdate();
  });
  auto local_parent = parent;
  // Allow the parent to be null -- for testing.
  if (local_parent != nullptr) {
    bind(wxEVT_CLOSE_WINDOW, [local_parent](wxCloseEvent &event) -> void {
      local_parent->onLibraryDialogClose();
    });
  }
}

void TeamLibraryDialog::onOk() {
  if (validateSettings()) {
    saveSettings();
    updateScoreTeams();
    close();
    return;
  }
}

void TeamLibraryDialog::onCancel() { close(); }

void TeamLibraryDialog::onAddOrUpdate() {
  proto::TeamInfo_TeamType type = proto::TeamInfo_TeamType_TEAM_ERROR;
  if (default_team_selector->selected() == "Home") {
    type = proto::TeamInfo_TeamType_HOME_TEAM;
  }
  if (default_team_selector->selected() == "Away") {
    type = proto::TeamInfo_TeamType_AWAY_TEAM;
  }
  if (row_for_edit == -1) {
    team_selection->addTeam(name_entry->value(),
                            FilesystemPath(file_name_entry->value()), type);
    return;
  }
  team_selection->changeTeam(row_for_edit, name_entry->value(),
                             FilesystemPath(file_name_entry->value()), type);
}

auto TeamLibraryDialog::validateSettings() -> bool { return true; }

void TeamLibraryDialog::saveSettings() { team_selection->saveLibrary(); }

void TeamLibraryDialog::clearEdit() {
  row_for_edit = -1;
  name_entry->setValue("");
  file_name_entry->setValue("");
  default_team_selector->setSelected(0);
  add_update_button->setText("Add");
}

void TeamLibraryDialog::editTeam(int32_t row_number, const std::string &name,
                                 const FilesystemPath &logo,
                                 proto::TeamInfo_TeamType type) {
  row_for_edit = row_number;
  name_entry->setValue(name);
  file_name_entry->setValue(logo.string());
  if (type == proto::TeamInfo_TeamType_HOME_TEAM) {
    default_team_selector->setSelected(1);
  } else if (type == proto::TeamInfo_TeamType_AWAY_TEAM) {
    default_team_selector->setSelected(2);
  } else {
    default_team_selector->setSelected(0);
  }
  add_update_button->setText("Update");
}

void TeamLibraryDialog::updateScoreTeams() {
  // Allow for a null parent if used with dialog-test.
  if (parent != nullptr) {
    parent->setTeams(team_selection->selectedTeams());
  }
}

}  // namespace cszb_scoreboard
