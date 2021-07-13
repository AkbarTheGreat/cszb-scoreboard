/*
ui/dialog/edit_image_library/FileListBox.h: A subclass of wxEditableListBox
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
#pragma once

#include <wx/editlbox.h>          // for wxEditableListBoxNameStr, wxEL_ALLO...
#include <stdint.h>               // for int32_t
#include <wx/defs.h>              // for wxWindowID
#include <wx/gdicmn.h>            // for wxPoint, wxSize (ptr only)
#include <wx/string.h>            // for wxString
#include <wx/tbarbase.h>          // for wxDefaultPosition, wxDefaultSize
#include <vector>                 // for vector

#include "util/FilesystemPath.h"  // for FilesystemPath

class wxCommandEvent;
class wxWindow;

namespace cszb_scoreboard {

const int32_t FILE_LIST_BOX_DEFAULT_STYLE = wxEL_ALLOW_NEW | wxEL_ALLOW_DELETE;

class FileListBox : public wxEditableListBox {
 public:
  FileListBox(wxWindow* parent, wxWindowID id, const wxString& label,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int32_t style = FILE_LIST_BOX_DEFAULT_STYLE,
              const wxString& name = wxEditableListBoxNameStr);

  auto getFilenames() -> std::vector<FilesystemPath>;
  auto selectedFilename() -> FilesystemPath;

 protected:
  void bindEvents();
  auto listSize() -> int32_t;
  void newPressed(wxCommandEvent& event);  // NOLINT(google-runtime-references)
                                           // wxWidgets callback.
  void selectItem(int32_t select_index);
  auto selectedIndex() -> int32_t;
  void updateStrings(const std::vector<FilesystemPath>& filenames,
                     int32_t select_index = 0);
};

}  // namespace cszb_scoreboard
