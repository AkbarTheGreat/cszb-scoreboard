/*
ui/component/control/ThingsMode.h: A control for managing 5/6 things in a
short-form improv show.

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

#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"
#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

class ThingsMode : public ScreenTextController {
 public:
  static ThingsMode *Create(PreviewPanel *preview_panel, wxWindow *parent);
  void textUpdated(wxKeyEvent &event);
  void updatePreview() override;

 private:
  wxAuiManager aui_manager;
  TeamSelector *screen_selection;
  wxScrolledWindow *scrollable_panel;
  wxButton *new_activity_button;
  wxButton *new_replacement_button;

  ActivityPanel *home_activities_panel;
  ActivityPanel *away_activities_panel;
  ActivityPanel *all_activities_panel;

  ThingsMode(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  ~ThingsMode() { aui_manager.UnInit(); }

  void createControls(wxPanel *control_panel) override;

  void addActivity(wxCommandEvent &event);
  void addReplacement(wxCommandEvent &event);
  void addToScrollable(wxWindow *item, int row);
  void bindEvents();
  void positionWidgets(wxPanel *control_panel);
  void screenChanged(wxCommandEvent &event);
  void updateScroll();
};

}  // namespace cszb_scoreboard
