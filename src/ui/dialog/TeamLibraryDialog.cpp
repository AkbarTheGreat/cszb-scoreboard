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

#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "config/swx/defs.h"     // for wxID_CANCEL, wxID_OK
#include "config/swx/event.h"    // for wxEVT_BUTTON
#include "ui/widget/DropDown.h"  // for DropDown
// IWYU pragma: no_include <google/protobuf/repeated_ptr_field.h>

namespace cszb_scoreboard {
class ScoreControl;

namespace swx {
class PropertySheetDialog;
}  // namespace swx

TeamLibraryDialog::TeamLibraryDialog(swx::PropertySheetDialog *wx,
                                     ScoreControl *parent, Singleton *singleton)
    : TabbedDialog(wx) {
  this->parent = parent;
  this->singleton = singleton;

  box_panel = panel();
  team_selection =
      std::make_unique<TeamSelectionBox>(box_panel->childPanel(), this);
  bottom_panel = box_panel->panel();

  divider = bottom_panel->divider();

  name_entry = bottom_panel->text("");
  name_label = bottom_panel->label("Team name");

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

  bottom_panel->runSizer();

  box_panel->addWidget(*team_selection, 0, 0);
  box_panel->addWidget(*bottom_panel, 1, 0);

  box_panel->runSizer();

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
