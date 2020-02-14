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

#include <wx/wx.h>

#include <vector>

#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"

namespace cszb_scoreboard {

class ThingsMode : public ScreenTextController {
 public:
  static ThingsMode *Create(PreviewPanel *preview_panel, wxWindow *parent);

 private:
  TeamSelector *screen_selection;
  wxButton *new_activity_button;
  wxButton *new_replacement_button;
  wxScrolledWindow *scrollable_panel;

  wxPanel *home_activities_panel;
  wxPanel *home_replacements_panel;
  std::vector<wxRadioButton *> home_activity_selectors;
  std::vector<wxTextCtrl *> home_activities;
  std::vector<wxButton *> home_remove_activity_buttons;
  std::vector<wxTextCtrl *> home_replacables;
  std::vector<wxTextCtrl *> home_replacements;
  std::vector<wxButton *> home_remove_replacement_buttons;

  wxPanel *away_activities_panel;
  wxPanel *away_replacements_panel;
  std::vector<wxRadioButton *> away_activity_selectors;
  std::vector<wxTextCtrl *> away_activities;
  std::vector<wxButton *> away_remove_activity_buttons;
  std::vector<wxTextCtrl *> away_replacables;
  std::vector<wxTextCtrl *> away_replacements;
  std::vector<wxButton *> away_remove_replacement_buttons;

  wxPanel *all_activities_panel;
  wxPanel *all_replacements_panel;
  std::vector<wxRadioButton *> all_activity_selectors;
  std::vector<wxTextCtrl *> all_activities;
  std::vector<wxButton *> all_remove_activity_buttons;
  std::vector<wxTextCtrl *> all_replacables;
  std::vector<wxTextCtrl *> all_replacements;
  std::vector<wxButton *> all_remove_replacement_buttons;

  ThingsMode(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}

  void updatePreview() override;
  void createControls(wxPanel *control_panel) override;

  void addActivity(wxCommandEvent &event);
  void addReplacement(wxCommandEvent &event);
  void bindEvents();
  void placeAndSizePanel(int rows, wxPanel *panel);
  void positionWidgets(wxPanel *control_panel);
  void screenChanged(wxCommandEvent &event);
  void textUpdated(wxKeyEvent &event);
  void updateScroll();
};

}  // namespace cszb_scoreboard
