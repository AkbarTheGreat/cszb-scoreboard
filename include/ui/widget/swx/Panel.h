/*
ui/widget/swx/Panel.h: A wrapper around wxPanel.

Copyright 2021-2022 Tracy Beck

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

class Panel {
 public:
  virtual auto asWx() -> wxPanel * { return nullptr; }
  virtual void setBackgroundStyle(int64_t style) = 0;
};

class PanelImpl : public Panel, public wxPanel {
 public:
  explicit PanelImpl(wxWindow *parent, wxWindowID id = wxID_ANY,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     int64_t style = wxTAB_TRAVERSAL,
                     const wxString &name = wxPanelNameStr)
      : wxPanel(parent, id, pos, size, style, name) {}
  auto asWx() -> wxPanel * override { return this; }
  void setBackgroundStyle(int64_t style) override {
    wxWindow::SetBackgroundStyle(static_cast<wxBackgroundStyle>(style));
  }
};

}  // namespace cszb_scoreboard::swx
