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

class Frame {
 public:
  virtual auto Close(bool force = false) -> bool = 0;
  virtual auto CreateStatusBar(int number = 1, long style = wxSTB_DEFAULT_STYLE,
                               wxWindowID id = 0,
                               const wxString &name = wxStatusBarNameStr)
      -> wxStatusBar * = 0;
  virtual auto Destroy() -> bool = 0;
  virtual auto GetPosition() const -> wxPoint = 0;
  virtual auto GetWindowStyle() const -> long = 0;
  virtual void Iconize(bool iconize = true) = 0;
  virtual void SetAcceleratorTable(const wxAcceleratorTable &accel) = 0;
  virtual void SetMenuBar(wxMenuBar *menuBar) = 0;
  virtual void SetPosition(const wxPoint &pt) = 0;
  virtual void SetSize(const wxSize &size) = 0;
  virtual void SetStatusText(const wxString &text, int number = 0) = 0;
  virtual void SetWindowStyle(long style) = 0;
  virtual auto Show(bool show = true) -> bool = 0;
  virtual auto ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL)
      -> bool = 0;
  virtual void Update() = 0;
};

class FrameImpl : public Frame, public wxFrame {
 public:
  FrameImpl(wxWindow *parent, wxWindowID id, const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            int64_t style = wxDEFAULT_FRAME_STYLE,
            const wxString &name = wxFrameNameStr)
      : wxFrame(parent, id, title, pos, size, style, name){};

  virtual auto Close(bool force = false) -> bool {
    return wxFrame::Close(force);
  }
  virtual auto CreateStatusBar(int number = 1, long style = wxSTB_DEFAULT_STYLE,
                               wxWindowID id = 0,
                               const wxString &name = wxStatusBarNameStr)
      -> wxStatusBar * {
    return wxFrame::CreateStatusBar(number, style, id, name);
  }
  virtual auto Destroy() -> bool { return wxFrame::Destroy(); }
  virtual auto GetPosition() const -> wxPoint { return wxFrame::GetPosition(); }
  virtual auto GetWindowStyle() const -> long {
    return wxFrame::GetWindowStyle();
  }
  virtual void Iconize(bool iconize = true) { wxFrame::Iconize(iconize); }
  virtual void SetAcceleratorTable(const wxAcceleratorTable &accel) {
    wxFrame::SetAcceleratorTable(accel);
  }
  virtual void SetMenuBar(wxMenuBar *menuBar) { wxFrame::SetMenuBar(menuBar); }
  virtual void SetPosition(const wxPoint &pt) { wxFrame::SetPosition(pt); }
  virtual void SetSize(const wxSize &size) { wxFrame::SetSize(size); }
  virtual void SetStatusText(const wxString &text, int number = 0) {
    wxFrame::SetStatusText(text, number);
  }
  virtual void SetWindowStyle(long style) { wxFrame::SetWindowStyle(style); }
  virtual auto Show(bool show = true) -> bool { return wxFrame::Show(show); }
  virtual auto ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL)
      -> bool {
    return wxFrame::ShowFullScreen(show, style);
  }
  virtual void Update() { wxFrame::Update(); }
};

}  // namespace cszb_scoreboard::swx
