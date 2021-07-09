/*
ui/component/control/things_mode/Activity.cpp: Represents an activity in 5/6
things.

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

#include "ui/component/control/things_mode/Activity.h"

#include <wx/wx.h>

#include <vector>

#include "ui/UiUtil.h"
#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

Activity::Activity(Widget *parent, Panel *activity_frame,
                   Panel *replacement_frame, int index, bool is_first) {
  this->index = index;
  this->parent = parent;

  control_pane = new wxPanel(activity_frame->wx);

  activity_selector =
      new wxRadioButton(control_pane, wxID_ANY, "", wxDefaultPosition,
                        wxDefaultSize, wxRB_SINGLE);
  if (is_first) {
    activity_selector->SetValue(true);
  }
  activity_text = new wxTextCtrl(control_pane, wxID_ANY, "", wxDefaultPosition,
                                 wxSize(-1, -1), wxTE_MULTILINE);
  up_button = new wxButton(control_pane, wxID_ANY, "^", wxDefaultPosition,
                           wxDefaultSize, wxBU_EXACTFIT);
  down_button = new wxButton(control_pane, wxID_ANY, "v", wxDefaultPosition,
                             wxDefaultSize, wxBU_EXACTFIT);
  remove_activity_button =
      new wxButton(control_pane, wxID_ANY, "X", wxDefaultPosition,
                   wxDefaultSize, wxBU_EXACTFIT);
  replacement_panel = new ReplacementPanel(replacement_frame->wx, parent);
  bindEvents();
  positionWidgets();
}

void Activity::copyFrom(Activity *other) {
  activity_selector->SetValue(other->activity_selector->GetValue());
  activity_text->SetValue(other->activity_text->GetValue());
  replacement_panel->copyFrom(other->replacement_panel);
}

Activity::~Activity() {
  delete replacement_panel;
  control_pane->Destroy();
}

void Activity::bindEvents() {
  activity_selector->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                          &ActivityPanel::selectionChanged,
                          dynamic_cast<ActivityPanel *>(parent));

  up_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Activity::moveButton, this);
  down_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &Activity::moveButton, this);

  remove_activity_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                               &ActivityPanel::deleteActivity,
                               dynamic_cast<ActivityPanel *>(parent));
  auto *ap = dynamic_cast<ActivityPanel *>(parent);
  activity_text->Bind(wxEVT_KEY_UP,
                      [ap](wxKeyEvent &event) -> void { ap->textUpdated(); });
}

void Activity::positionWidgets() {
  wxSizer *sizer = UiUtil::sizer(
      0, 5);  // NOLINT(readability-magic-numbers) Matches the number of columns
              // for the items listed below this line.
  sizer->Add(activity_selector, 0, wxALL, BORDER_SIZE);
  sizer->Add(activity_text, 0, wxALL, BORDER_SIZE);
  sizer->Add(up_button, 0, wxALL, BORDER_SIZE);
  sizer->Add(down_button, 0, wxALL, BORDER_SIZE);
  sizer->Add(remove_activity_button, 0, wxALL, BORDER_SIZE);
  control_pane->SetSizerAndFit(sizer);
}

void Activity::moveButton(wxCommandEvent &event) {
  if (event.GetEventObject() == up_button) {
    dynamic_cast<ActivityPanel *>(parent)->swapActivities(index, index - 1);
  } else if (event.GetEventObject() == down_button) {
    dynamic_cast<ActivityPanel *>(parent)->swapActivities(index, index + 1);
  }
}

void Activity::select() {
  if (!activity_selector->GetValue()) {
    activity_selector->SetValue(true);
    wxCommandEvent event;
    event.SetEventObject(activity_selector);
    dynamic_cast<ActivityPanel *>(parent)->selectionChanged(event);
  }
}

void Activity::unselect() { activity_selector->SetValue(false); }

auto Activity::isSelected() -> bool { return activity_selector->GetValue(); }

auto Activity::resolveSelection(wxObject *selected_object) -> bool {
  if (activity_selector == selected_object) {
    select();
    return true;
  }
  unselect();
  return false;
}

void Activity::setIndex(int index, int max_index) {
  this->index = index;
  if (index == 0) {
    up_button->Disable();
  } else {
    up_button->Enable();
  }
  if (index == max_index) {
    down_button->Disable();
  } else {
    down_button->Enable();
  }
}

auto Activity::containsDeleteButton(wxObject *delete_button) -> bool {
  return (delete_button == remove_activity_button);
}

void Activity::selectionChanged(wxCommandEvent &event) {
  dynamic_cast<ActivityPanel *>(parent)->selectionChanged(event);
}

auto Activity::previewText() -> std::string {
  if (activity_text->GetValue().empty()) {
    return " ";
  }
  return activity_text->GetValue().ToStdString();
}

}  // namespace cszb_scoreboard
