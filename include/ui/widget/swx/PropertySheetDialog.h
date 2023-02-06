/*
ui/widget/swx/PropertySheetDialog.h: A wrapper around wxPropertySheetDialog.

Copyright 2021-2023 Tracy Beck

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

#include <wx/propdlg.h>
#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class PropertySheetDialog : public wxPropertySheetDialog {
 public:
  PropertySheetDialog(wxWindow *parent, wxWindowID id, const wxString &title,
                      const wxPoint &pos = wxDefaultPosition,
                      const wxSize &size = wxDefaultSize,
                      int64_t style = wxDEFAULT_DIALOG_STYLE,
                      const wxString &name = wxDialogNameStr)
      : wxPropertySheetDialog(parent, id, title, pos, size, style, name) {}

  auto ProcessEvent(wxEvent &event) -> bool override {
    return wxPropertySheetDialog::ProcessEvent(event);
  }
};

}  // namespace cszb_scoreboard::swx
