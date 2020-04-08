/*
ui/dialog/edit_image_library/FileListBox.cpp: A subclass of wxEditableListBox
which specifically contains a list of files.

Copyright 2020 Tracy Beck

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

#include <wx/listctrl.h>

#include "config/ImageLibrary.h"

namespace cszb_scoreboard {

FileListBox::FileListBox(wxWindow* parent, wxWindowID id, const wxString& label,
                         const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
    : wxEditableListBox(parent, id, label, pos, size, style, name) {
  updateStrings(ImageLibrary::getInstance()->allFilenames());
  bindEvents();
}

void FileListBox::bindEvents() {
  GetNewButton()->Bind(wxEVT_BUTTON, &FileListBox::newPressed, this);
}

void FileListBox::newPressed(wxCommandEvent& event) {
  wxFileDialog dialog(this, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  std::vector<FilesystemPath> filenames = getFilenames();
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath new_file = (std::string)dialog.GetPath();
    // Insert the new file after the currently selected one.
    long new_index = selectedIndex() + 1;
    if (selectedIndex() >= listSize()) {
      new_index = listSize();
    }
    filenames.insert(std::next(filenames.begin(), new_index), new_file);
    updateStrings(filenames, new_index);
  }
}

std::vector<FilesystemPath> FileListBox::getFilenames() {
  wxArrayString strings;
  GetStrings(strings);
  std::vector<FilesystemPath> filenames;

  for (auto entry : strings) {
    if (entry != "") {
      filenames.push_back(FilesystemPath(std::string(entry)));
    }
  }

  return filenames;
}

long FileListBox::listSize() {
  wxArrayString strings;
  GetStrings(strings);
  return strings.GetCount();
}

FilesystemPath FileListBox::selectedFilename() {
  long index = selectedIndex();
  if (index == -1 || index >= listSize()) {
    return FilesystemPath();
  }
  return getFilenames()[index];
}

long FileListBox::selectedIndex() {
  if (listSize() == 0) {
    return -1;
  }
  return GetListCtrl()->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

void FileListBox::updateStrings(const std::vector<FilesystemPath>& filenames,
                                long select_index) {
  wxArrayString strings;
  for (auto file : filenames) {
    strings.Add(file.c_str());
  }
  SetStrings(strings);

  if (strings.GetCount() > 0) {
    selectItem(select_index);
  }
}

void FileListBox::selectItem(long select_index) {
  if (listSize() <= select_index) {
    select_index = 0;
  }

  GetListCtrl()->SetItemState(select_index, wxLIST_STATE_SELECTED,
                              wxLIST_STATE_SELECTED);
}

}  // namespace cszb_scoreboard
