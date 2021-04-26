/*
ui/component/ScreenPreview.h: This class manages the thumbnail preview of a
monitor in the main window.  In addition, the preview owns the ScreenPresenter
which displays to the actual monitor, dispatching the updated view when
necessary.

Copyright 2019-2021 Tracy Beck

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

#include "ui/component/ScreenPresenter.h"
#include "ui/component/ScreenThumbnail.h"

namespace cszb_scoreboard {
class ScreenPreview {
 public:
  ScreenPreview(wxWindow* parent, std::vector<proto::ScreenSide> sides,
                int monitor_number);
  auto widget() -> ScreenText*;
  void sendToPresenter(ScreenText* screen_text);
  void sendToPresenter();
  void blackoutPresenter();
  auto controlPane() -> wxPanel*;
  void resetFromSettings(int monitor_number);
  auto thumbnailWidget() -> ScreenText*;

 private:
  wxPanel* control_pane;
  ScreenText* current_widget;
  const wxWindow* parent;
  ScreenPresenter* presenter;
  ScreenThumbnail* thumbnail;
  static auto previewSize(int monitor_number) -> wxSize;
  void positionWidgets();
};
}  // namespace cszb_scoreboard
