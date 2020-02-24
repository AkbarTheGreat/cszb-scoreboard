/*
ui/component/control/things_mode/ActivityPanel.h: Represents all activities in
5/6 things.

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

#include "ui/component/control/things_mode/Activity.h"

namespace cszb_scoreboard {
class ActivityPanel : public wxPanel {
 public:
  ActivityPanel(wxWindow *parent, ScreenTextController *owning_controller);
  ~ActivityPanel();
  void addActivity(wxPanel *parent_panel);
  void deleteActivity(wxCommandEvent &event);
  void selectionChanged(wxCommandEvent &event);
  ReplacementPanel *replacementPanel();

 private:
  wxPanel *activity_side;
  wxPanel *replacement_side;
  std::vector<Activity *> activities;
  ScreenTextController *owning_controller;
  wxWindow *parent;

  void bindEvents();
  void positionWidgets();
};
}  // namespace cszb_scoreboard
