/*
ui/dialog/edit_image_library/FileListBox.cpp: A subclass of wxEditableListBox
which specifically contains a list of files.

Copyright 2020-2021 Tracy Beck

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

#include "ui/dialog/edit_image_library/FileListBox.h"

#include <iterator>  // for next
#include <optional>  // for optional
#include <string>    // for string

#include "ScoreboardCommon.h"      // for IMAGE_SELECTION_STRING
#include "config/ImageLibrary.h"   // for ImageLibrary
#include "ui/widget/FilePicker.h"  // for FilePicker

namespace cszb_scoreboard {

const int64_t FILE_LIST_BOX_DEFAULT_STYLE = wxEL_ALLOW_NEW | wxEL_ALLOW_DELETE;

FileListBox::FileListBox(swx::Panel *wx, const std::string &title) : Panel(wx) {
  box = listBox(title);
  updateStrings(ImageLibrary::getInstance()->allFilenames());
  bindEvents();
  addWidget(*box, 0, 0, 0);
  runSizer();
}

void FileListBox::bindEvents() {
  box->bindNew(wxEVT_BUTTON,
               [this](wxCommandEvent &event) -> void { this->newPressed(); });
}

void FileListBox::newPressed() {
  std::unique_ptr<FilePicker> dialog =
      openFilePicker("Select Image", IMAGE_SELECTION_STRING);
  std::vector<FilesystemPath> filenames = getFilenames();
  std::optional<FilesystemPath> new_file = dialog->selectFile();
  if (new_file.has_value()) {
    // Insert the new file after the currently selected one.
    int64_t new_index = box->selectedIndex() + 1;
    if (box->selectedIndex() >= box->listSize()) {
      new_index = box->listSize();
    }
    filenames.insert(std::next(filenames.begin(), new_index), *new_file);
    updateStrings(filenames, new_index);
  }
}

auto FileListBox::getFilenames() -> std::vector<FilesystemPath> {
  std::vector<std::string> strings = box->strings();
  std::vector<FilesystemPath> filenames;

  for (const auto &entry : strings) {
    if (!entry.empty()) {
      filenames.emplace_back(FilesystemPath(std::string(entry)));
    }
  }

  return filenames;
}

auto FileListBox::selectedFilename() -> FilesystemPath {
  int64_t index = box->selectedIndex();
  if (index == -1 || index >= box->listSize()) {
    return FilesystemPath();
  }
  return getFilenames()[index];
}

void FileListBox::updateStrings(const std::vector<FilesystemPath> &filenames,
                                int64_t select_index) {
  std::vector<std::string> strings;
  strings.reserve(filenames.size());
  for (const auto &file : filenames) {
    strings.push_back(file.string());
  }
  box->setStrings(strings);

  if (!strings.empty()) {
    box->selectItem(select_index);
  }
}

}  // namespace cszb_scoreboard
