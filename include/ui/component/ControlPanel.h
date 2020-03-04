/*
ui/component/ControlPanel.h: Holds the controls for changing the contents on a
screen.

Copyright 2019-2020 Tracy Beck

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

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include <vector>

#include "ScoreboardCommon.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"

namespace cszb_scoreboard {

class ControlPanel : public wxAuiNotebook {
 public:
  ControlPanel(wxWindow* parent, PreviewPanel* preview_panel);

#ifdef SCOREBOARD_TESTING
  ScreenTextController* textController(int index) { return controllers[index]; }
#endif

 private:
  void bindEvents();
  void tabChanged(wxAuiNotebookEvent& event);
  std::vector<ScreenTextController*> controllers;
};

}  // namespace cszb_scoreboard
