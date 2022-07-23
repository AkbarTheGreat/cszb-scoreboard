/*
ui/widget/Widget.h: Methods common to all of our widgets.

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

#include <functional>  // for function

#include "ScoreboardCommon.h"     // for DEFAULT_BORDER_SIZE
#include "config/swx/defs.h"      // for wxID_ANY, wxALL, wxGROW
#include "config/swx/event.h"     // for wxEventTypeTag, wxCloseEvent (ptr o...
#include "ui/widget/swx/Sizer.h"  // for Sizer
#include "wx/window.h"            // for wxWindow

namespace cszb_scoreboard {
class RenderContext;

const int NO_BORDER = 0;

class Widget {
 public:
  void addWidget(const Widget &widget, int row, int column,
                 int border_size = DEFAULT_BORDER_SIZE,
                 int flag = wxALL | wxGROW) {
    addWidgetWithSpan(widget, row, column, 1, 1, border_size, flag);
  }
  void addWidgetWithSpan(const Widget &widget, int row, int column,
                         int row_span, int column_span,
                         int border_size = DEFAULT_BORDER_SIZE,
                         int flag = wxALL | wxGROW);
  void bind(const wxEventTypeTag<wxCloseEvent> &eventType,
            const std::function<void(wxCloseEvent &)> &lambda,
            int id = wxID_ANY) const {
    wx()->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxCommandEvent> &eventType,
            const std::function<void(wxCommandEvent &)> &lambda,
            int id = wxID_ANY) const {
    wx()->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxKeyEvent> &eventType,
            const std::function<void(wxKeyEvent &)> &lambda,
            int id = wxID_ANY) const {
    wx()->Bind(eventType, lambda, id);
  }
  void bind(const wxEventTypeTag<wxMouseEvent> &eventType,
            const std::function<void(wxMouseEvent &)> &lambda,
            int id = wxID_ANY) const {
    wx()->Bind(eventType, lambda, id);
  }
  // A wxPaintEvent wraps more functionality than others, in that it creates a
  // renderable context for use in rendering to the window implicitly, which may
  // be utilized by other Widget methods.
  void bind(const wxEventTypeTag<wxPaintEvent> &eventType,
            const std::function<void(RenderContext *)> &lambda,
            int id = wxID_ANY);
  void focus() const { wx()->SetFocus(); }
  void moveWidget(Widget *widget, int row, int column);
  void removeColumnFromSizer(int column);
  void removeRowFromSizer(int row);
  void runSizer();
  // Reparents this widget to another widget for layout purposes
  void setParent(Widget *parent) const { wx()->Reparent(parent->wx()); }

  // This should be moved to protected when the conversion is complete.
  auto sizer() -> swx::Sizer *;

  [[nodiscard]] virtual auto wx() const -> wxWindow * = 0;

 private:
  // wxWidget will clean up our sizer for us, so keep it as an unmanaged
  // pointer.
  swx::Sizer *window_sizer = nullptr;
  auto widgetAtIndex(int row, int column) -> wxWindow *;
};

}  // namespace cszb_scoreboard
