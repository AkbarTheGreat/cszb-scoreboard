/*
ui/widget/Frame.h: A frame represents a free-standing window that is not a
dialog.

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

#include <vector>

#include "ui/widget/swx/Frame.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class Frame {
 public:
  explicit Frame(const wxString &title, const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize)
      : Frame(new swx::Frame(nullptr, wxID_ANY, title, pos, size)) {}
  // Injectable constructor, for testing.
  explicit Frame(swx::Frame *frame) { wx = frame; }
  virtual ~Frame() { wx->Destroy(); }

  void focus() { wx->SetFocus(); };
  void installHotkeys(const std::vector<wxAcceleratorEntry> &keys) {
    wx->SetAcceleratorTable(wxAcceleratorTable(keys.size(), keys.data()));
  }
  auto position() -> wxPoint { return wx->GetPosition(); }
  void setStatusBar(const wxString &text);
  auto show(bool show) -> bool { return wx->Show(show); }
  void updateWindow() { wx->Update(); }

 protected:
  void alwaysOnTop(bool isOnTop);
  void closeWindow() { wx->Close(true); }
  void fullScreen(bool show) { wx->ShowFullScreen(show); }
  void minimize() { wx->Iconize(); }
  void menuBar(const std::vector<std::pair<wxMenu *, std::string>> &menu);
  void setDimensions(const wxRect &dim);
  void setSizer(wxSizer *sizer) { wx->SetSizerAndFit(sizer); }

  void bind(const wxEventTypeTag<wxCommandEvent> &eventType,
            const std::function<void(wxCommandEvent &)> &lambda, int id) {
    wx->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxCloseEvent> &eventType,
            const std::function<void(wxCloseEvent &)> &lambda) {
    wx->Bind(eventType, lambda);
  }

  auto childPanel(wxWindowID id = wxID_ANY,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL,
                  const wxString &name = wxPanelNameStr) -> swx::Panel * {
    return new swx::Panel(wx, id, pos, size, style, name);
  }

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Frame *wx;

 private:
  bool hasStatusBar = false;
};

}  // namespace cszb_scoreboard
