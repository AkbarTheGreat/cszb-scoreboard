/*
ui/component/control/things_mode/Activity.cpp: Represents an activity in 5/6
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

#include "ui/component/control/things_mode/Activity.h"

#include <wx/wx.h>

#include <vector>

#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

Activity::Activity(wxWindow *parent, wxPanel *activity_frame,
                   wxPanel *replacement_frame, bool is_first) {
  this->parent = parent;

  control_pane = new wxPanel(activity_frame);

  activity_selector =
      new wxRadioButton(control_pane, wxID_ANY, "", wxDefaultPosition,
                        wxDefaultSize, wxRB_SINGLE);
  if (is_first) {
    activity_selector->SetValue(true);
  }
  activity_text = new wxTextCtrl(control_pane, wxID_ANY, "", wxDefaultPosition,
                                 wxSize(-1, -1), wxTE_MULTILINE);
  remove_activity_button =
      new wxButton(control_pane, wxID_ANY, "X", wxDefaultPosition,
                   wxDefaultSize, wxBU_EXACTFIT);
  replacement_panel = new ReplacementPanel(replacement_frame, parent);
  bindEvents();
  positionWidgets();
}

Activity::~Activity() {
  delete replacement_panel;
  control_pane->Destroy();
}

void Activity::bindEvents() {
  activity_selector->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                          &ActivityPanel::selectionChanged,
                          (ActivityPanel *)parent);

  remove_activity_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                               &ActivityPanel::deleteActivity,
                               (ActivityPanel *)parent);
  activity_text->Bind(wxEVT_KEY_UP, &ActivityPanel::textUpdated,
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

void Activity::select() {
  if (!activity_selector->GetValue()) {
    activity_selector->SetValue(true);
    wxCommandEvent event;
    event.SetEventObject(activity_selector);
    ((ActivityPanel *)parent)->selectionChanged(event);
  }
}

void Activity::unselect() { activity_selector->SetValue(false); }

bool Activity::isSelected() { return activity_selector->GetValue(); }

bool Activity::resolveSelection(wxObject *selected_object) {
  if (activity_selector == selected_object) {
    select();
    return true;
  } else {
    unselect();
    return false;
  }
}
bool Activity::containsDeleteButton(wxObject *delete_button) {
  if (delete_button == remove_activity_button) {
    return true;
  } else {
    return false;
  }
}

void Activity::selectionChanged(wxCommandEvent &event) {
  ((ActivityPanel *)parent)->selectionChanged(event);
}

std::string Activity::previewText() {
  if (activity_text->GetValue() == "") {
    return " ";
  }
  return activity_text->GetValue().ToStdString();
}

}  // namespace cszb_scoreboard
