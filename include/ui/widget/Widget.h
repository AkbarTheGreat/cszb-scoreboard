/*
ui/widget/Widget.h: Methods common to all of our widgets.

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

#include <wx/gbsizer.h>
#include <wx/wx.h>

#include "ScoreboardCommon.h"
#include "ui/widget/swx/Sizer.h"

namespace cszb_scoreboard {

class Widget {
 public:
  void addWidget(Widget *widget, int row, int column, int row_span = 1,
                 int column_span = 1, int border_size = DEFAULT_BORDER_SIZE,
                 int flag = wxALL | wxGROW);
  void bind(const wxEventTypeTag<wxCloseEvent> &eventType,
            const std::function<void(wxCloseEvent &)> &lambda,
            int id = wxID_ANY) {
    _wx()->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxCommandEvent> &eventType,
            const std::function<void(wxCommandEvent &)> &lambda,
            int id = wxID_ANY) {
    _wx()->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxMouseEvent> &eventType,
            const std::function<void(wxMouseEvent &)> &lambda,
            int id = wxID_ANY) {
    _wx()->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxPaintEvent> &eventType,
            const std::function<void(wxPaintEvent &)> &lambda,
            int id = wxID_ANY) {
    _wx()->Bind(eventType, lambda, id);
  }
  void runSizer() { _wx()->SetSizerAndFit(sizer()); }

  // This should be moved to protected when the conversion is complete.
  auto sizer() -> swx::Sizer *;

 protected:
  virtual wxWindow *_wx() = 0;

 private:
  // wxWidget will clean up our sizer for us, so keep it as an unmanaged
  // pointer.
  swx::Sizer *window_sizer = nullptr;
};

}  // namespace cszb_scoreboard
