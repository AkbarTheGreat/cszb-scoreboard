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

class Replacement {
 public:
  Replacement(wxWindow *parent, ScreenTextController *owning_controller);
  std::vector<wxWindow *> line();
  static int lineWidth();

 private:
  ScreenTextController *owning_controller;
  wxTextCtrl *replaceable;
  wxTextCtrl *replacement;
  wxButton *remove_replacement_button;
  wxStaticText *spacer_text;

  void bindEvents();
  void deleteReplacement(wxCommandEvent &event);
};

class ReplacementsPanel : public wxPanel {
 public:
  ReplacementsPanel(wxWindow *parent, ScreenTextController *owning_controller);

 private:
  ScreenTextController *owning_controller;
  std::vector<Replacement> replacements;

  void bindEvents();
  void positionWidgets();
};

class Activity {
 public:
  Activity(wxWindow *parent, wxWindow *parent_frame,
           ScreenTextController *owning_controller, bool is_first);
  bool isSelected();
  std::vector<wxWindow *> line();
  static int lineWidth();
  ReplacementsPanel *replacementsPanel() { return replacement_panel; }
  void select();

 private:
  wxRadioButton *activity_selector;
  wxTextCtrl *activity_text;
  wxWindow *parent;
  wxButton *remove_activity_button;
  ReplacementsPanel *replacement_panel;
  ScreenTextController *owning_controller;

  void bindEvents();
  void selectionChanged(wxCommandEvent &event);
};

class ActivitiesPanel : public wxPanel {
 public:
  ActivitiesPanel(wxWindow *parent, ScreenTextController *owning_controller);
  void addActivity(wxPanel *parent_panel);
  void selectionChanged(wxCommandEvent &event);
  ReplacementsPanel *replacementsPanel();

 private:
  std::vector<Activity> activities;
  ScreenTextController *owning_controller;
  wxWindow *parent;

  void bindEvents();
  void positionWidgets();
};

class ThingsMode : public ScreenTextController {
 public:
  static ThingsMode *Create(PreviewPanel *preview_panel, wxWindow *parent);
  void textUpdated(wxKeyEvent &event);
  void updatePreview() override;

 private:
  TeamSelector *screen_selection;
  wxButton *new_activity_button;
  wxButton *new_replacement_button;
  wxScrolledWindow *scrollable_panel;

  ActivitiesPanel *home_activities_panel;
  ActivitiesPanel *away_activities_panel;
  ActivitiesPanel *all_activities_panel;

  ThingsMode(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}

  void createControls(wxPanel *control_panel) override;

  void addActivity(wxCommandEvent &event);
  void addReplacement(wxCommandEvent &event);
  void bindEvents();
  void positionWidgets(wxPanel *control_panel);
  void screenChanged(wxCommandEvent &event);
  void updateScroll();
};

}  // namespace cszb_scoreboard
