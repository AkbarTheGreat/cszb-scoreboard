/*
ui/widget/swx/CheckBox.h: A wrapper around wxCheckBox.

Copyright 2021-2025 Tracy Beck

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

#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class CheckBox : public wxCheckBox {
 public:
  CheckBox(wxWindow *parent, wxWindowID id, const wxString &label,
           const wxPoint &pos = wxDefaultPosition,
           const wxSize &size = wxDefaultSize, int64_t style = 0,
           const wxValidator &validator = wxDefaultValidator,
           const wxString &name = wxCheckBoxNameStr)
      : wxCheckBox(parent, id, label, pos, size, style, validator, name) {}
};

}  // namespace cszb_scoreboard::swx
