/*
ui/widget/swx/TextCtrl.h: A wrapper around wxTextCtrl.

Copyright 2021-2024 Tracy Beck

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

#include <wx/tglbtn.h>
#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class TextCtrl : public wxTextCtrl {
 public:
  TextCtrl(wxWindow *parent, wxWindowID id,
           const wxString &value = wxEmptyString,
           const wxPoint &pos = wxDefaultPosition,
           const wxSize &size = wxDefaultSize, int64_t style = 0,
           const wxValidator &validator = wxDefaultValidator,
           const wxString &name = wxTextCtrlNameStr)
      : wxTextCtrl(parent, id, value, pos, size, style, validator, name) {}
};

}  // namespace cszb_scoreboard::swx
