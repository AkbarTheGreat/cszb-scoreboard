/*
ui/widget/swx/Frame.h: A wrapper around wxFrame

Copyright 2021 Tracy Beck

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

class Frame : public wxFrame {
 public:
  Frame(wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString &name = wxFrameNameStr)
      : wxFrame(parent, id, title, pos, size, style, name){};
};

}  // namespace cszb_scoreboard::swx
