/*
ui/component/QuickStatePanel.h: This panel provides quick-settable screens.

Copyright 2020 Tracy Beck

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

#include "ui/component/ScreenText.h"

namespace cszb_scoreboard {

class QuickStatePanel : public wxPanel {
 public:
  QuickStatePanel(wxWindow* parent);

 private:
  ~QuickStatePanel();
  void bindEvents();
  void positionWidgets();
  void executeShortcut(wxMouseEvent& event);
  void setShortcut(wxMouseEvent& event);

  std::vector<ScreenText*> screen_texts;
};

}  // namespace cszb_scoreboard
