/*
ui/widget/swx/StaticText.h: A wrapper around wxStaticText.

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

class StaticText : public wxStaticText {
 public:
  StaticText(wxWindow *parent, wxWindowID id, const wxString &label,
             const wxPoint &pos = wxDefaultPosition,
             const wxSize &size = wxDefaultSize, int64_t style = 0,
             const wxString &name = wxStaticTextNameStr)
      : wxStaticText(parent, id, label, pos, size, style, name) {}
};

}  // namespace cszb_scoreboard::swx
