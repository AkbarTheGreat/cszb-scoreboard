/*
ui/component/control/things_mode/ActivityPanel.cpp: Represents all activities in 5/6 things.

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

#include "ui/component/control/things_mode/ActivityPanel.h"
#include "ScoreboardCommon.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ActivityPanel::ActivityPanel(wxWindow *parent,
                                 ScreenTextController *owning_controller)
    : wxPanel(parent) {
  this->owning_controller = owning_controller;
  this->parent = parent;
  activities.push_back(Activity(this, parent, owning_controller, true));
  bindEvents();
  positionWidgets();
}

void ActivityPanel::addActivity(wxPanel *parent_panel) {
  bool is_first = (activities.empty());
  activities.push_back(
      Activity(this, parent_panel, owning_controller, is_first));
  for (auto widget : activities.back().line()) {
    GetSizer()->Add(widget, 0, wxALL, BORDER_SIZE);
  }
  parent_panel->GetSizer()->Add(activities.back().replacementPanel(), 0, wxALL,
                                BORDER_SIZE);
  activities.back().select();
  selectionChanged(wxCommandEvent());
  SetSizerAndFit(GetSizer());
}

void ActivityPanel::selectionChanged(wxCommandEvent &event) {
  for (auto activity : activities) {
    if (activity.isSelected()) {
      activity.replacementPanel()->Show();
    } else {
      activity.replacementPanel()->Hide();
    }
  }
  owning_controller->updatePreview();
}

void ActivityPanel::bindEvents() {}

ReplacementPanel *ActivityPanel::replacementPanel() {
  if (activities.empty()) {
    return nullptr;
  }

  for (auto activity : activities) {
    if (activity.isSelected()) {
      return activity.replacementPanel();
    }
  }

  // Should be unreachable.
  return activities[0].replacementPanel();
}

void ActivityPanel::positionWidgets() {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, Activity::lineWidth(), 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto activity : activities) {
    for (auto widget : activity.line()) {
      sizer->Add(widget, 0, wxALL, BORDER_SIZE);
    }
  }
  SetSizerAndFit(sizer);
}
}  // namespace cszb_scoreboard
