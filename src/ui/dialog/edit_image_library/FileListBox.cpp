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

#include <wx/arrstr.h>       // for wxArrayString
#include <wx/bmpbuttn.h>     // for wxBitmapButton
#include <wx/event.h>        // for wxCommandEvent (ptr only), wxEVT_BU...
#include <wx/listctrl.h>     // IWYU pragma: keep for wxListCtrl
#include <wx/string.h>       // for wxString
#include <wx/translation.h>  // for _

#include <algorithm>  // for max
#include <iterator>   // for next
#include <string>     // for string

#include "ScoreboardCommon.h"     // for IMAGE_SELECTION_STRING
#include "config/ImageLibrary.h"  // for ImageLibrary
#include "ui/widget/swx/Panel.h"  // for Panel
#include "wx/filedlg.h"           // for wxFileDialog, wxFD_FILE_MUST_EXIST
// IWYU pragma: no_include <wx/generic/listctrl.h>
// IWYU pragma: no_include <wx/gtk/bmpbuttn.h>

namespace cszb_scoreboard {

FileListBox::FileListBox(swx::Panel *wx, const std::string &title) : Panel(wx) {
  box = listBox(title);
  updateStrings(ImageLibrary::getInstance()->allFilenames());
  bindEvents();
  addWidget(*box, 0, 0, 0);
  runSizer();
}

void FileListBox::bindEvents() {
  box->bindNew(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->newPressed(event);
  });
}

void FileListBox::newPressed(wxCommandEvent &event) {
  wxFileDialog dialog(wx, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  std::vector<FilesystemPath> filenames = getFilenames();
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath new_file = FilesystemPath(std::string(dialog.GetPath()));
    // Insert the new file after the currently selected one.
    int32_t new_index = box->selectedIndex() + 1;
    if (box->selectedIndex() >= box->listSize()) {
      new_index = box->listSize();
    }
    filenames.insert(std::next(filenames.begin(), new_index), new_file);
    updateStrings(filenames, new_index);
  }
}

auto FileListBox::getFilenames() -> std::vector<FilesystemPath> {
  wxArrayString strings;
  box->strings(strings);
  std::vector<FilesystemPath> filenames;

  for (const auto &entry : strings) {
    if (!entry.empty()) {
      filenames.emplace_back(FilesystemPath(std::string(entry)));
    }
  }

  return filenames;
}

auto FileListBox::selectedFilename() -> FilesystemPath {
  int32_t index = box->selectedIndex();
  if (index == -1 || index >= box->listSize()) {
    return FilesystemPath();
  }
  return getFilenames()[index];
}

void FileListBox::updateStrings(const std::vector<FilesystemPath> &filenames,
                                int32_t select_index) {
  wxArrayString strings;
  for (const auto &file : filenames) {
    strings.Add(file.c_str());
  }
  box->setStrings(strings);

  if (strings.GetCount() > 0) {
    box->selectItem(select_index);
  }
}

}  // namespace cszb_scoreboard
