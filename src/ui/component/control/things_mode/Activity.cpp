/*
ui/component/control/things_mode/Activity.h: Represents an activity in 5/6 things.

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
#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

Activity::Activity(wxWindow *parent, wxWindow *top_frame,
                   ScreenTextController *owning_controller, bool is_first) {
  this->owning_controller = owning_controller;
  this->parent = parent;

  if (is_first) {
    activity_selector = new wxRadioButton(
        parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  } else {
    activity_selector = new wxRadioButton(parent, wxID_ANY, "");
  }
  activity_text = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                                 wxSize(-1, -1), wxTE_MULTILINE);
  remove_activity_button = new wxButton(
      parent, wxID_ANY, "X", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  replacement_panel = new ReplacementPanel(top_frame, owning_controller);
  bindEvents();
}

void Activity::select() { activity_selector->SetValue(true); }

bool Activity::isSelected() { return activity_selector->GetValue(); }

std::vector<wxWindow *> Activity::line() {
  std::vector<wxWindow *> list_of_widgets;
  list_of_widgets.push_back(activity_selector);
  list_of_widgets.push_back(activity_text);
  list_of_widgets.push_back(remove_activity_button);
  return list_of_widgets;
}

int Activity::lineWidth() { return 3; }

void Activity::bindEvents() {
  activity_selector->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                          &ActivityPanel::selectionChanged,
                          (ActivityPanel *)parent);
}

void Activity::selectionChanged(wxCommandEvent &event) {
  ((ActivityPanel *)parent)->selectionChanged(event);
}
}  // namespace cszb_scorebaord
