/*
ui/dialog/EditImageLibraryDialog.h: A dialog which allows a user to
add/remove/edit images in the image library.

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
#include <wx/propdlg.h>
#include <wx/wx.h>

#include "image_library.pb.h"
#include "ui/dialog/edit_image_library/FileListBox.h"
#include "util/FilesystemPath.h"

namespace cszb_scoreboard {

class EditImageLibraryDialog : public wxPropertySheetDialog {
 public:
  bool Create(wxWindow* parent);

 private:
  FileListBox* file_list;
  wxTextCtrl* name_entry;
  wxStaticText* name_label;
  wxEditableListBox* tag_list;
  std::map<FilesystemPath, proto::ImageInfo> images;
  wxPanel* panel;
  wxWindow* parent;

  void bindEvents();
  void fileSelected(wxListEvent& event);
  void onOk(wxCommandEvent& event);
  void onCancel(wxCommandEvent& event);
  void onClose(wxCloseEvent& event);
  void nameUpdated(wxKeyEvent& event);
  void positionWidgets();
  void saveSettings();
  void tagDeleted(wxListEvent& event);
  void tagsUpdated(wxListEvent& event);
  bool validateSettings();
};

}  // namespace cszb_scoreboard
