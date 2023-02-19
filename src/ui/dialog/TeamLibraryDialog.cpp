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

#include <wx/string.h>  // for wxString

#include <cstdint>     // for int32_t, int64_t
#include <filesystem>  // for operator==, path
#include <optional>    // for optional
#include <string>      // for string
#include <vector>      // for vector

#include "ScoreboardCommon.h"                          // for DEFAULT_BORDER...
#include "config/swx/defs.h"                           // for wxID_CANCEL
#include "config/swx/event.h"                          // for wxListEvent
#include "ui/component/control/ImageFromLibrary.h"     // for ImageFromLibrary
#include "ui/dialog/edit_image_library/FileListBox.h"  // for FileListBox
#include "ui/widget/DirectoryPicker.h"                 // for DirectoryPicker
#include "util/Log.h"                                  // for LogDebug
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace cszb_scoreboard {
namespace swx {
class PropertySheetDialog;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

TeamLibraryDialog::TeamLibraryDialog(swx::PropertySheetDialog *wx,
                                     ScoreControl *parent, Singleton *singleton)
    : TabbedDialog(wx) {
  this->parent = parent;
  this->singleton = singleton;

  library = proto::TeamLibrary();  // TODO: Load/Save with Persistence.cpp

  box_panel = panel();
  name_entry = box_panel->text("Rozzie Square Pegs");
  name_label = box_panel->label("Team name");

  file_name_label = box_panel->label("Logo Filename");
  file_name_entry = box_panel->text("<filename>");

  positionWidgets();
  bindEvents();
}

void TeamLibraryDialog::positionWidgets() {
  int32_t row = 0;
  box_panel->addWidgetWithSpan(*name_label, ++row, 0, 1, 1);
  box_panel->addWidgetWithSpan(*name_entry, row, 1, 1, 3);

  box_panel->addWidgetWithSpan(*file_name_label, ++row, 0, 1, 1);
  box_panel->addWidgetWithSpan(*file_name_entry, row, 1, 1, 3);

  box_panel->runSizer();

  addPage(*box_panel, "");
  runSizer();
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
  // TODO: Save
}

}  // namespace cszb_scoreboard
