/*
ui/dialog/edit_image_library/FileListBox.h: A subclass of wxEditableListBox
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
#pragma once

#include <wx/editlbox.h>
#include <wx/wx.h>

#include "image_library.pb.h"
#include "util/FilesystemPath.h"

namespace cszb_scoreboard {

const long FILE_LIST_BOX_DEFAULT_STYLE = wxEL_ALLOW_NEW | wxEL_ALLOW_DELETE;

class FileListBox : public wxEditableListBox {
 public:
  FileListBox(wxWindow* parent, wxWindowID id, const wxString& label,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = FILE_LIST_BOX_DEFAULT_STYLE,
              const wxString& name = wxEditableListBoxNameStr);

  std::vector<FilesystemPath> getFilenames();
  FilesystemPath selectedFilename();

 protected:
  std::vector<FilesystemPath> filenames;

  void bindEvents();
  void newPressed(wxCommandEvent& event);
  void updateStrings();
};

}  // namespace cszb_scoreboard
