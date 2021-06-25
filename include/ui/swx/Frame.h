/*
ui/swx/Frame.h: A wrapper around wxFrame

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

namespace cszb_scoreboard {

class SwxFrame {
 public:
  SwxFrame(wxWindowID id, const wxString &title,
           const wxPoint &pos = wxDefaultPosition,
           const wxSize &size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString &name = wxFrameNameStr) {
    wx = new wxFrame(nullptr, id, title, pos, size, style, name);
  }
  virtual ~SwxFrame() { wx->Destroy(); }

  virtual bool Close(bool force) { return wx->Close(force); }
  virtual bool Destroy() { return wx->Destroy(); }
  virtual wxStatusBar *CreateStatusBar() { return wx->CreateStatusBar(); }
  virtual wxPoint GetPosition() { return wx->GetPosition(); }
  virtual long GetWindowStyle() { return wx->GetWindowStyle(); }
  virtual void Iconize() { wx->Iconize(); }
  virtual void SetMenuBar(wxMenuBar *menuBar) { wx->SetMenuBar(menuBar); }
  virtual void SetPosition(const wxPoint &pt) { wx->SetPosition(pt); }
  virtual void SetSize(const wxRect &rect) { wx->SetSize(rect); }
  virtual void SetSizerAndFit(wxSizer *sizer) { wx->SetSizerAndFit(sizer); }
  virtual void SetStatusText(const wxString &text) { wx->SetStatusText(text); }
  virtual void SetWindowStyle(long style) { wx->SetWindowStyle(style); }
  virtual bool Show(bool show) { return wx->Show(show); }
  virtual bool ShowFullScreen(bool show) { return wx->ShowFullScreen(show); }
  virtual void Bind(const wxEventTypeTag<wxCommandEvent> &eventType,
                    const std::function<void(wxCommandEvent &)> &lambda,
                    int id) {
    wx->Bind(eventType, lambda, id);
  }
  virtual void Bind(const wxEventTypeTag<wxCloseEvent> &eventType,
                    const std::function<void(wxCloseEvent &)> &lambda) {
    wx->Bind(eventType, lambda);
  }

  // TODO(akbar):  Once everything is a unique_ptr, FrameList should be less
  // useful and we can remove it and change this to private.
 protected:
  wxFrame *wx;
};

}  // namespace cszb_scoreboard
