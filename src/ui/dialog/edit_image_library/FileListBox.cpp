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
#include <wx/bmpbuttn.h>     // IWYU pragma: keep for wxBitmapButton
#include <wx/event.h>        // for wxEventTypeTag, wxCommandEvent, wxE...
#include <wx/filedlg.h>      // for wxFileDialog
#include <wx/listctrl.h>     // IWYU pragma: keep for wxListCtrl
#include <wx/listctrl.h>     // IWYU pragma: keep for wxListCtrl
#include <wx/translation.h>  // for _

#include <algorithm>  // for max
#include <iterator>   // for next
#include <string>     // for string

#include "ScoreboardCommon.h"     // for IMAGE_SELECTION_STRING
#include "config/ImageLibrary.h"  // for ImageLibrary
#include "wx/filedlg.h"           // for wxFD_FILE_MUST_EXIST, wxFD_OPEN
// IWYU pragma: no_include <wx/generic/listctrl.h>
// IWYU pragma: no_include <wx/gtk/bmpbuttn.h>

class wxWindow;

namespace cszb_scoreboard {

FileListBox::FileListBox(wxWindow *parent, wxWindowID id, const wxString &label,
                         const wxPoint &pos, const wxSize &size, int32_t style,
                         const wxString &name)
    : wxEditableListBox(parent, id, label, pos, size, style, name) {
  updateStrings(ImageLibrary::getInstance()->allFilenames());
  bindEvents();
}

void FileListBox::bindEvents() {
  GetNewButton()->Bind(wxEVT_BUTTON, &FileListBox::newPressed, this);
}

void FileListBox::newPressed(wxCommandEvent &event) {
  wxFileDialog dialog(this, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  std::vector<FilesystemPath> filenames = getFilenames();
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath new_file = FilesystemPath(std::string(dialog.GetPath()));
    // Insert the new file after the currently selected one.
    int32_t new_index = selectedIndex() + 1;
    if (selectedIndex() >= listSize()) {
      new_index = listSize();
    }
    filenames.insert(std::next(filenames.begin(), new_index), new_file);
    updateStrings(filenames, new_index);
  }
}

auto FileListBox::getFilenames() -> std::vector<FilesystemPath> {
  wxArrayString strings;
  GetStrings(strings);
  std::vector<FilesystemPath> filenames;

  for (const auto &entry : strings) {
    if (!entry.empty()) {
      filenames.emplace_back(FilesystemPath(std::string(entry)));
    }
  }

  return filenames;
}

auto FileListBox::listSize() -> int32_t {
  wxArrayString strings;
  GetStrings(strings);
  return strings.GetCount();
}

auto FileListBox::selectedFilename() -> FilesystemPath {
  int32_t index = selectedIndex();
  if (index == -1 || index >= listSize()) {
    return FilesystemPath();
  }
  return getFilenames()[index];
}

auto FileListBox::selectedIndex() -> int32_t {
  if (listSize() == 0) {
    return -1;
  }
  return GetListCtrl()->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

void FileListBox::updateStrings(const std::vector<FilesystemPath> &filenames,
                                int32_t select_index) {
  wxArrayString strings;
  for (const auto &file : filenames) {
    strings.Add(file.c_str());
  }
  SetStrings(strings);

  if (strings.GetCount() > 0) {
    selectItem(select_index);
  }
}

void FileListBox::selectItem(int32_t select_index) {
  if (listSize() <= select_index) {
    select_index = 0;
  }

  GetListCtrl()->SetItemState(select_index, wxLIST_STATE_SELECTED,
                              wxLIST_STATE_SELECTED);
}

}  // namespace cszb_scoreboard
