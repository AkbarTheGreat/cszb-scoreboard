/*
ui/widget/swx/Frame.h: A wrapper around wxFrame

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

class Frame {
 public:
  virtual auto Close(bool force) -> bool = 0;
  auto Close() -> bool { return Close(false); }
  virtual auto CreateStatusBar(int number, int64_t style, wxWindowID id,
                               const wxString &name) -> wxStatusBar * = 0;
  auto CreateStatusBar(int number = 1, int64_t style = wxSTB_DEFAULT_STYLE,
                       wxWindowID id = 0) -> wxStatusBar * {
    return CreateStatusBar(number, style, id, wxStatusBarNameStr);
  }
  virtual auto Destroy() -> bool = 0;
  virtual auto EnableFullScreenView(bool enable=true) -> bool = 0;
  [[nodiscard]] virtual auto GetPosition() const -> wxPoint = 0;
  [[nodiscard]] virtual auto GetWindowStyle() const -> int64_t = 0;
  virtual void Iconize(bool iconize) = 0;
  void Iconize() { Iconize(true); }
  virtual void Refresh() = 0;
  virtual void SetAcceleratorTable(const wxAcceleratorTable &accel) = 0;
  virtual void SetMenuBar(wxMenuBar *menuBar) = 0;
  virtual void SetPosition(const wxPoint &pt) = 0;
  virtual void SetSize(const wxSize &size) = 0;
  virtual void SetStatusText(const wxString &text, int number) = 0;
  void SetStatusText(const wxString &text) { SetStatusText(text, 0); }
  virtual void SetWindowStyle(int64_t style) = 0;
  virtual auto Show(bool show) -> bool = 0;
  auto Show() -> bool { return Show(true); }
  virtual auto ShowFullScreen(bool show, int64_t style) -> bool = 0;
  auto ShowFullScreen(bool show) -> bool {
    return ShowFullScreen(show, wxFULLSCREEN_ALL);
  }
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

  auto Close(bool force) -> bool override { return wxFrame::Close(force); }
  auto CreateStatusBar(int number, int64_t style, wxWindowID id,
                       const wxString &name) -> wxStatusBar * override {
    return wxFrame::CreateStatusBar(number, style, id, name);
  }
  auto Destroy() -> bool override { return wxFrame::Destroy(); }
  auto EnableFullScreenView(bool enable) -> bool override { return wxFrame::EnableFullScreenView(enable); }
  [[nodiscard]] auto GetPosition() const -> wxPoint override {
    return wxFrame::GetPosition();
  }
  [[nodiscard]] auto GetWindowStyle() const -> int64_t override {
    return wxFrame::GetWindowStyle();
  }
  void Iconize(bool iconize) override { wxFrame::Iconize(iconize); }
  void Refresh() override { wxFrame::Refresh(); }
  void SetAcceleratorTable(const wxAcceleratorTable &accel) override {
    wxFrame::SetAcceleratorTable(accel);
  }
  void SetMenuBar(wxMenuBar *menuBar) override { wxFrame::SetMenuBar(menuBar); }
  void SetPosition(const wxPoint &pt) override { wxFrame::SetPosition(pt); }
  void SetSize(const wxSize &size) override { wxFrame::SetSize(size); }
  void SetStatusText(const wxString &text, int number) override {
    wxFrame::SetStatusText(text, number);
  }
  void SetWindowStyle(int64_t style) override {
    wxFrame::SetWindowStyle(style);
  }
  auto Show(bool show) -> bool override { return wxFrame::Show(show); }
  auto ShowFullScreen(bool show, int64_t style) -> bool override {
    return wxFrame::ShowFullScreen(show, style);
  }
  void Update() override { wxFrame::Update(); }
};

}  // namespace cszb_scoreboard::swx
