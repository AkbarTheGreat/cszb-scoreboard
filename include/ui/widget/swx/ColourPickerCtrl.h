/*
ui/widget/swx/ColourPickerCtrl.h: A wrapper around wxColourPickerCtrl.

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

#include <wx/clrpicker.h>
#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class ColourPickerCtrl : public wxColourPickerCtrl {
 public:
  ColourPickerCtrl(wxWindow *parent, wxWindowID id,
                   const wxColour &colour = *wxBLACK,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   int64_t style = wxCLRP_DEFAULT_STYLE,
                   const wxValidator &validator = wxDefaultValidator,
                   const wxString &name = wxColourPickerCtrlNameStr)
      : wxColourPickerCtrl(parent, id, colour, pos, size, style, validator,
                           name) {}
};

}  // namespace cszb_scoreboard::swx
