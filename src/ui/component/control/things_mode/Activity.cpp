/*
ui/component/control/things_mode/Activity.h: Represents an activity in 5/6
things.

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

#include "ui/component/control/things_mode/Activity.h"

#include <wx/wx.h>

#include <vector>

#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

Activity::Activity(wxWindow *parent, ScreenTextController *owning_controller,
                   bool is_first) {
  this->owning_controller = owning_controller;
  this->parent = parent;

  control_pane = new wxPanel(parent);

  if (is_first) {
    activity_selector =
        new wxRadioButton(control_pane, wxID_ANY, "", wxDefaultPosition,
                          wxDefaultSize, wxRB_GROUP);
  } else {
    activity_selector = new wxRadioButton(control_pane, wxID_ANY, "");
  }
  activity_text = new wxTextCtrl(control_pane, wxID_ANY, "", wxDefaultPosition,
                                 wxSize(-1, -1), wxTE_MULTILINE);
  remove_activity_button =
      new wxButton(control_pane, wxID_ANY, "X", wxDefaultPosition,
                   wxDefaultSize, wxBU_EXACTFIT);
  replacement_panel = new ReplacementPanel(parent, owning_controller);
  bindEvents();
  positionWidgets();
}

void Activity::select() { activity_selector->SetValue(true); }

bool Activity::isSelected() { return activity_selector->GetValue(); }

void Activity::bindEvents() {
  activity_selector->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                          &ActivityPanel::selectionChanged,
                          (ActivityPanel *)parent);
}

void Activity::positionWidgets() {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 3, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  sizer->Add(activity_selector, 0, wxALL, BORDER_SIZE);
  sizer->Add(activity_text, 0, wxALL, BORDER_SIZE);
  sizer->Add(remove_activity_button, 0, wxALL, BORDER_SIZE);

  control_pane->SetSizerAndFit(sizer);
}

void Activity::selectionChanged(wxCommandEvent &event) {
  ((ActivityPanel *)parent)->selectionChanged(event);
}
}  // namespace cszb_scoreboard
