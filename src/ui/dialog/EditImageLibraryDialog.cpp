/*
ui/dialog/EditImageLibraryDialog.cpp: A dialog which allows a user to
add/remove/edit images in the image library.

Copyright 2020-2022 Tracy Beck

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

#include "ui/dialog/EditImageLibraryDialog.h"

#include <wx/string.h>  // for wxString

#include <filesystem>  // for operator<
#include <string>      // for string, basic_...
#include <vector>      // for vector

#include "ScoreboardCommon.h"                          // for DEFAULT_BORDER...
#include "config/swx/defs.h"                           // for wxID_CANCEL
#include "config/swx/event.h"                          // for wxListEvent
#include "ui/component/control/ImageFromLibrary.h"     // for ImageFromLibrary
#include "ui/dialog/edit_image_library/FileListBox.h"  // for FileListBox
#include "util/Log.h"
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace cszb_scoreboard {
namespace swx {
class PropertySheetDialog;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

EditImageLibraryDialog::EditImageLibraryDialog(swx::PropertySheetDialog *wx,
                                               ImageFromLibrary *parent,
                                               Singleton *singleton)
    : TabbedDialog(wx) {
  this->parent = parent;
  this->singleton = singleton;

  library = singleton->imageLibrary()->temporaryClone();
  box_panel = panel();
  file_list =
      std::make_unique<FileListBox>(box_panel->childPanel(), "Filename",
                                    singleton->imageLibrary()->allFilenames());

  name_entry = box_panel->text("");
  name_label = box_panel->label("Display name");

  root_divider = box_panel->divider();
  root_entry =
      box_panel->text(singleton->imageLibrary()->libraryRoot().string());
  root_label = box_panel->label("Library root");
  root_move_checkbox =
      box_panel->checkBox("Have files already moved to new root?");

  tag_list = box_panel->listBox("Tags");

  positionWidgets();
  bindEvents();
}

void EditImageLibraryDialog::positionWidgets() {
  box_panel->addWidgetWithSpan(*file_list, 0, 0, 1, 2);
  box_panel->addWidget(*tag_list, 0, 2);
  box_panel->addWidget(*name_label, 1, 0);
  box_panel->addWidgetWithSpan(*name_entry, 1, 1, 1, 2);
  box_panel->addWidgetWithSpan(*root_divider, 2, 0, 1, 3);
  box_panel->addWidget(*root_label, 3, 0);
  box_panel->addWidgetWithSpan(*root_entry, 3, 1, 1, 2);
  box_panel->addWidgetWithSpan(*root_move_checkbox, 4, 0, 1, 3);

  box_panel->runSizer();

  addPage(*box_panel, "");

  runSizer();
}

void EditImageLibraryDialog::bindEvents() {
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onOk(); },
      wxID_OK);
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onCancel(); },
      wxID_CANCEL);
  ImageFromLibrary *local_parent = parent;
  bind(wxEVT_CLOSE_WINDOW, [local_parent](wxCloseEvent &event) -> void {
    local_parent->onEditDialogClose();
  });
  file_list->bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent &event) -> void {
    this->fileSelected(&event);
  });
  name_entry->bind(wxEVT_KEY_UP,
                   [this](wxKeyEvent &event) -> void { this->nameUpdated(); });
  root_entry->bind(wxEVT_KEY_UP,
                   [this](wxKeyEvent &event) -> void { this->rootUpdated(); });
  tag_list->bind(wxEVT_LIST_END_LABEL_EDIT, [this](wxListEvent &event) -> void {
    this->tagsUpdated(event);
  });
  tag_list->bind(wxEVT_LIST_DELETE_ITEM, [this](wxListEvent &event) -> void {
    this->tagDeleted(event);
  });
  file_list->setChangeCallback(
      [this](const FilesystemPath &prev, const FilesystemPath curr) -> void {
        this->fileUpdated(prev, curr);
      });
}

void EditImageLibraryDialog::onOk() {
  if (validateSettings()) {
    saveSettings();
    close();
    return;
  }
}

void EditImageLibraryDialog::onCancel() { close(); }

auto EditImageLibraryDialog::validateSettings() -> bool { return true; }

void EditImageLibraryDialog::saveSettings() {
  singleton->imageLibrary()->copyFrom(*library.get());
  singleton->imageLibrary()->saveLibrary();
}

void EditImageLibraryDialog::fileUpdated(const FilesystemPath &prev,
                                         const FilesystemPath &curr) {
  FilesystemPath empty("");
  if (prev == empty && curr == empty) {
    LogDebug(
        "fileUpdated called with empty/empty, unsure what to do with two empty "
        "paths.");
    return;
  }
  if (prev == empty) {
    library->addImage(curr, "", {});
    // New file.
    return;
  }
  if (curr == empty) {
    library->deleteImage(prev);
    // Delete file.
    return;
  }
  // Updating existing file.
  library->moveImage(prev, curr);
}

void EditImageLibraryDialog::fileSelected(wxListEvent *event) {
  FilesystemPath filename = file_list->selectedFilename();

  name_entry->setValue(library->name(filename));
  std::vector<std::string> tags;
  for (const auto &tag : library->tags(filename)) {
    tags.push_back(tag.string());
  }
  tag_list->setStrings(tags);
  event->Skip();
}

void EditImageLibraryDialog::nameUpdated() {
  library->setName(file_list->selectedFilename(), name_entry->value());
}

void EditImageLibraryDialog::rootUpdated() {}

void EditImageLibraryDialog::tagDeleted(const wxListEvent &event) {
  std::vector<std::string> tags = tag_list->strings();

  tags.erase(tags.begin() + event.GetIndex());

  tag_list->setStrings(tags);

  FilesystemPath filename = file_list->selectedFilename();
  library->setTags(filename, tags);
}

void EditImageLibraryDialog::tagsUpdated(const wxListEvent &event) {
  std::vector<std::string> tags = tag_list->strings();
  int index = event.GetIndex();
  if (index >= tags.size()) {
    tags.push_back(event.GetText().ToStdString());
  } else {
    tags[index] = event.GetText();
  }
  tag_list->setStrings(tags);

  FilesystemPath filename = file_list->selectedFilename();
  library->setTags(filename, tags);
}

}  // namespace cszb_scoreboard
