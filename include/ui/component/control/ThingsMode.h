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

namespace cszb_scoreboard {

class ThingsMode : public ScreenTextController {
 public:
  ThingsMode(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenTextController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<ThingsMode>;
  void textUpdated(wxKeyEvent &event);  // NOLINT(google-runtime-references)
                                        // wxWidgets callback.
  void updateScreenText(ScreenText *screen_text) override;

 private:
  wxPanel *button_panel;
  wxButton *new_activity_button;
  wxButton *new_replacement_button;
  wxRadioBox *presenter_selection;
  TeamSelector *screen_selection;
  wxScrolledWindow *scrollable_panel;

  ActivityPanel *home_activities_panel;
  ActivityPanel *away_activities_panel;
  ActivityPanel *all_activities_panel;

  void createControls(Panel *control_panel) override;

  void bindEvents();
  void positionWidgets(Panel *control_panel);
  void updateActivityPanel();
  // wxWidgets callbacks, waive linting error for references.
  void addActivity(wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void addReplacement(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void presentedListChanged(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void screenChanged();
};

}  // namespace cszb_scoreboard
