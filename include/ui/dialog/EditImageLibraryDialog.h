/*
ui/dialog/EditImageLibraryDialog.h: A dialog which allows a user to
add/remove/edit images in the image library.

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

#include <wx/editlbox.h>
#include <wx/propdlg.h>
#include <wx/wx.h>
#include <map>

#include "image_library.pb.h"
#include "ui/dialog/edit_image_library/FileListBox.h"
#include "util/FilesystemPath.h"

class wxCloseEvent;
class wxCommandEvent;
class wxEditableListBox;
class wxKeyEvent;
class wxListEvent;
class wxPanel;
class wxStaticText;
class wxTextCtrl;
class wxWindow;

namespace cszb_scoreboard {
class FileListBox;

class EditImageLibraryDialog : public wxPropertySheetDialog {
 public:
  auto Create(wxWindow* parent) -> bool;

 private:
  FileListBox* file_list;
  wxTextCtrl* name_entry;
  wxStaticText* name_label;
  wxEditableListBox* tag_list;
  std::map<FilesystemPath, proto::ImageInfo> images;
  wxPanel* panel;
  wxWindow* parent;

  void bindEvents();
  void positionWidgets();
  void saveSettings();
  static auto validateSettings() -> bool;
  // wxWidgets callbacks, waive linting error for references.
  void fileSelected(wxListEvent& event);  // NOLINT(google-runtime-references)
  void onOk(wxCommandEvent& event);       // NOLINT(google-runtime-references)
  void onCancel(wxCommandEvent& event);   // NOLINT(google-runtime-references)
  void onClose(wxCloseEvent& event);      // NOLINT(google-runtime-references)
  void nameUpdated(wxKeyEvent& event);    // NOLINT(google-runtime-references)
  void tagDeleted(wxListEvent& event);    // NOLINT(google-runtime-references)
  void tagsUpdated(wxListEvent& event);   // NOLINT(google-runtime-references)
};

}  // namespace cszb_scoreboard
