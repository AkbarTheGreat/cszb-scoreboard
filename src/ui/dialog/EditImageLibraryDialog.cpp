/*
ui/dialog/EditImageLibraryDialog.cpp: A dialog which allows a user to
add/remove/edit images in the image library.

Copyright 2020-2023 Tracy Beck

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

EditImageLibraryDialog::EditImageLibraryDialog(swx::PropertySheetDialog *wx,
                                               ImageFromLibrary *parent,
                                               Singleton *singleton)
    : TabbedDialog(wx) {
  this->parent = parent;
  this->singleton = singleton;

  library = singleton->imageLibrary()->temporaryClone();
  box_panel = panel();
  file_list = std::make_unique<FileListBox>(box_panel->childPanel(), "Filename",
                                            library->allFilenames());

  name_entry = box_panel->text("");
  name_label = box_panel->label("Display Name");

  full_name_label = box_panel->label("Filename");
  full_name_entry = box_panel->text("");
  full_name_entry->disable();

  root_divider = box_panel->divider();
  root_entry =
      box_panel->text(singleton->imageLibrary()->libraryRoot().string());
  root_entry->disable();
  root_browse = box_panel->button("Library Root", true);
  root_browse->toolTip("Set a new root directory for your library.");
  root_clear = box_panel->button("Clear Root", true);
  root_clear->toolTip(
      "Remove the root directory for your library, all paths will be "
      "absolute.");

  tag_list = box_panel->listBox("Tags");

  positionWidgets();
  bindEvents();
}

void EditImageLibraryDialog::positionWidgets() {
  int32_t row = 0;
  box_panel->addWidgetWithSpan(*file_list, row, 0, 1, 2);
  box_panel->addWidgetWithSpan(*tag_list, row, 2, 1, 2);

  box_panel->addWidgetWithSpan(*name_label, ++row, 0, 1, 1);
  box_panel->addWidgetWithSpan(*name_entry, row, 1, 1, 3);

  box_panel->addWidgetWithSpan(*full_name_label, ++row, 0, 1, 1);
  box_panel->addWidgetWithSpan(*full_name_entry, row, 1, 1, 3);

  box_panel->addWidgetWithSpan(*root_divider, ++row, 0, 1, 4);

  box_panel->addWidgetWithSpan(*root_browse, ++row, 0, 1, 1);
  box_panel->addWidgetWithSpan(*root_entry, row, 1, 1, 3);

  box_panel->addWidgetWithSpan(*root_clear, ++row, 0, 1, 1);

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
  // Allow the parent to be null -- for testing.
  if (local_parent != nullptr) {
    bind(wxEVT_CLOSE_WINDOW, [local_parent](wxCloseEvent &event) -> void {
      local_parent->onEditDialogClose();
    });
  }
  file_list->bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent &event) -> void {
    this->fileSelected(&event);
  });
  name_entry->bind(wxEVT_KEY_UP,
                   [this](wxKeyEvent &event) -> void { this->nameUpdated(); });
  root_browse->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->rootBrowsePressed();
  });
  root_clear->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->rootClearPressed();
  });
  tag_list->bind(wxEVT_LIST_END_LABEL_EDIT, [this](wxListEvent &event) -> void {
    this->tagsUpdated(event);
  });
  tag_list->bind(wxEVT_LIST_DELETE_ITEM, [this](wxListEvent &event) -> void {
    this->tagDeleted(event);
  });
  file_list->setChangeCallback(
      [this](const FilesystemPath &prev, const FilesystemPath &curr) -> void {
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
  singleton->imageLibrary()->copyFrom(*library);
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
    // New file.
    std::string name = bestGuessImageName(curr);
    library->addImage(curr, name, {});
    file_list->selectIndex(library->allFilenames().size() - 1);
    refreshFiles();
    return;
  }
  if (curr == empty) {
    // Delete file.
    library->deleteImage(prev);
    refreshFiles();
    return;
  }
  // Updating existing file.
  library->moveImage(prev, curr);
  refreshFiles();
}

void EditImageLibraryDialog::fileSelected(wxListEvent *event) {
  FilesystemPath filename = file_list->selectedFilename();

  name_entry->setValue(library->name(filename));
  full_name_entry->setValue(FilesystemPath::absolutePath(
      library->libraryRoot().string(), filename.string()));
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

void EditImageLibraryDialog::refreshFiles() {
  int64_t selected = file_list->selectedIndex();
  file_list->setFilenames(library->allFilenames());
  file_list->selectIndex(selected);
}

void EditImageLibraryDialog::rootBrowsePressed() {
  std::unique_ptr<DirectoryPicker> dialog =
      box_panel->openDirectoryPicker("Select New Root", library->libraryRoot());
  std::optional<FilesystemPath> new_root = dialog->selectDirectory();
  if (new_root.has_value()) {
    // Replace the existing root with the new one.
    root_entry->setValue(new_root->string());
    library->smartUpdateLibraryRoot(*new_root);
    refreshFiles();
  }
}

void EditImageLibraryDialog::rootClearPressed() {
  root_entry->setValue("");
  library->setLibraryRoot(FilesystemPath(""));
  refreshFiles();
}

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

auto EditImageLibraryDialog::bestGuessImageName(const FilesystemPath &file)
    -> std::string {
  return "";
}

}  // namespace cszb_scoreboard
