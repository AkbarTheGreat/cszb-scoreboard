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

#include "config/ImageLibrary.h"

namespace cszb_scoreboard {

FileListBox::FileListBox(wxWindow* parent, wxWindowID id, const wxString& label,
                         const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
    : wxEditableListBox(parent, id, label, pos, size, style, name) {
  filenames = ImageLibrary::getInstance()->allFilenames();
  updateStrings();
  bindEvents();
}

void FileListBox::bindEvents() {
  GetNewButton()->Bind(wxEVT_BUTTON, &FileListBox::newPressed, this);
}

void FileListBox::newPressed(wxCommandEvent& event) {
  wxFileDialog dialog(this, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath selected_file = (std::string)dialog.GetPath();
    filenames.push_back(selected_file);
  }
  updateStrings();
}

std::vector<FilesystemPath> FileListBox::getFilenames() { return filenames; }

FilesystemPath FileListBox::selectedFilename() {
  // TODO: This is wrong.
  return filenames[0];
}

void FileListBox::updateStrings() {
  wxArrayString strings;
  for (auto file : filenames) {
    strings.Add(file.c_str());
  }
  SetStrings(strings);
}

}  // namespace cszb_scoreboard
