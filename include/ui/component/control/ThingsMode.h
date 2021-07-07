/*
ui/component/control/ThingsMode.h: A control for managing 5/6 things in a
short-form improv show.

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

#include <vector>

#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"
#include "ui/component/control/things_mode/ActivityPanel.h"
#include "ui/widget/Button.h"
#include "ui/widget/Panel.h"
#include "ui/widget/Radio.h"

namespace cszb_scoreboard {

class ThingsMode : public ScreenTextController {
 public:
  ThingsMode(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenTextController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<ThingsMode>;
  void textUpdated();
  void updateScreenText(ScreenText *screen_text) override;

 private:
  std::unique_ptr<Panel> button_panel;
  std::unique_ptr<Button> new_activity_button;
  std::unique_ptr<Button> new_replacement_button;
  std::unique_ptr<Radio> presenter_selection;
  std::unique_ptr<TeamSelector> screen_selection;
  wxScrolledWindow *scrollable_panel;

  ActivityPanel *home_activities_panel;
  ActivityPanel *away_activities_panel;
  ActivityPanel *all_activities_panel;

  void createControls(Panel *control_panel) override;

  void bindEvents();
  void positionWidgets(Panel *control_panel);
  void updateActivityPanel();
  void addActivity();
  void addReplacement();
  void presentedListChanged();
  void screenChanged();
};

}  // namespace cszb_scoreboard
