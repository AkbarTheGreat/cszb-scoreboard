/*
ui/component/control/ThingsMode.cpp: A control for managing 5/6 things in a
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

#include "ui/component/control/ThingsMode.h"

#include "config/TeamConfig.h"
#include "util/ProtoUtil.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;
const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ThingsMode *ThingsMode::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  ThingsMode *entry = new ThingsMode(preview_panel, parent);
  entry->initializeWidgets();
  entry->updatePreview();
  return entry;
}

void ThingsMode::createControls(wxPanel *control_panel) {
  scrollable_panel = new wxScrolledWindow(
      control_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);

  screen_selection = new TeamSelector(scrollable_panel);

  new_activity_button =
      new wxButton(scrollable_panel, wxID_ANY, "New Activity");
  new_replacement_button =
      new wxButton(scrollable_panel, wxID_ANY, "New Replacement");

  home_activities_panel = new ActivitiesPanel(scrollable_panel, this);
  away_activities_panel = new ActivitiesPanel(scrollable_panel, this);
  all_activities_panel = new ActivitiesPanel(scrollable_panel, this);

  positionWidgets(control_panel);
  bindEvents();
}

void ThingsMode::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *outer_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  outer_sizer->SetFlexibleDirection(wxBOTH);
  outer_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *scrollable_sizer = new wxFlexGridSizer(0, 2, 0, 0);
  scrollable_sizer->SetFlexibleDirection(wxBOTH);
  scrollable_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  scrollable_sizer->Add(new_activity_button, 0, wxALL, BORDER_SIZE);
  scrollable_sizer->Add(new_replacement_button, 0, wxALL, BORDER_SIZE);

  scrollable_sizer->Add(home_activities_panel);
  scrollable_sizer->Add(home_activities_panel->replacementsPanel());
  scrollable_sizer->Add(away_activities_panel);
  scrollable_sizer->Add(away_activities_panel->replacementsPanel());
  scrollable_sizer->Add(all_activities_panel);
  scrollable_sizer->Add(all_activities_panel->replacementsPanel());

  screenChanged(wxCommandEvent());

  scrollable_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);

  scrollable_panel->SetSizer(scrollable_sizer);
  scrollable_panel->FitInside();
  scrollable_panel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);

  outer_sizer->Add(scrollable_panel, 0, wxALL, BORDER_SIZE);
  control_panel->SetSizerAndFit(outer_sizer);
}

void ThingsMode::bindEvents() {
  new_activity_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                            &ThingsMode::addActivity, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &ThingsMode::screenChanged, this);
}

void ThingsMode::updatePreview() {
  // Send the combined text to both previews
  if (screen_selection->allSelected()) {
    proto::ScreenSide side;
    side.set_home(true);
    side.set_away(true);
    // previewPanel()->setTextForPreview(all_text, all_font_size, all_color,
    // false, side);
  } else {
  }
}

void ThingsMode::textUpdated(wxKeyEvent &event) { updatePreview(); }

void ThingsMode::screenChanged(wxCommandEvent &event) {
  if (screen_selection->allSelected()) {
    home_activities_panel->Hide();
    home_activities_panel->replacementsPanel()->Hide();
    away_activities_panel->Hide();
    away_activities_panel->replacementsPanel()->Hide();
    all_activities_panel->Show();
    all_activities_panel->replacementsPanel()->Show();
  } else if (screen_selection->homeSelected()) {
    away_activities_panel->Hide();
    away_activities_panel->replacementsPanel()->Hide();
    all_activities_panel->Hide();
    all_activities_panel->replacementsPanel()->Hide();
    home_activities_panel->Show();
    home_activities_panel->replacementsPanel()->Show();
  } else if (screen_selection->awaySelected()) {
    home_activities_panel->Hide();
    home_activities_panel->replacementsPanel()->Hide();
    all_activities_panel->Hide();
    all_activities_panel->replacementsPanel()->Hide();
    away_activities_panel->Show();
    away_activities_panel->replacementsPanel()->Show();
  }

  updatePreview();
}

void ThingsMode::addActivity(wxCommandEvent &event) {
  home_activities_panel->addActivity(scrollable_panel);

  scrollable_panel->SetSizer(scrollable_panel->GetSizer());
  scrollable_panel->FitInside();
  scrollable_panel->SetScrollRate(0, scrollable_panel->GetSize().GetHeight());
  scrollable_panel->SetSizeHints(scrollable_panel->GetSize());
  //  scrollable_panel->SetSize(scrollable_panel->GetVirtualSize().GetWidth(),
  //                            scrollable_panel->GetSize().GetHeight());

  control_panel->SetSizerAndFit(control_panel->GetSizer());
}

void ThingsMode::addReplacement(wxCommandEvent &event) {}

Replacement::Replacement(wxWindow *parent,
                         ScreenTextController *owning_controller) {
  this->owning_controller = owning_controller;
  replaceable = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, -1), wxTE_MULTILINE);
  replacement = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, -1), wxTE_MULTILINE);
  remove_replacement_button = new wxButton(
      parent, wxID_ANY, "X", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  // We add a blank space to the end of the line so that when the scrollbar
  // appears it doesn't occlude the delete button.
  spacer_text = new wxStaticText(parent, wxID_ANY, "   ");
  bindEvents();
}

std::vector<wxWindow *> Replacement::line() {
  std::vector<wxWindow *> list_of_widgets;
  list_of_widgets.push_back(replaceable);
  list_of_widgets.push_back(replacement);
  list_of_widgets.push_back(remove_replacement_button);
  list_of_widgets.push_back(spacer_text);
  return list_of_widgets;
}

int Replacement::lineWidth() { return 4; }

void Replacement::bindEvents() {}

void Replacement::deleteReplacement(wxCommandEvent &event) {}

ReplacementsPanel::ReplacementsPanel(wxWindow *parent,
                                     ScreenTextController *owning_controller)
    : wxPanel(parent) {
  this->owning_controller = owning_controller;
  replacements.push_back(Replacement(this, owning_controller));
  bindEvents();
  positionWidgets();
}

void ReplacementsPanel::bindEvents() {}

void ReplacementsPanel::positionWidgets() {
  wxFlexGridSizer *sizer =
      new wxFlexGridSizer(0, Replacement::lineWidth(), 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto replacement : replacements) {
    for (auto widget : replacement.line()) {
      sizer->Add(widget, 0, wxALL, BORDER_SIZE);
    }
  }
  SetSizerAndFit(sizer);
}

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
  replacement_panel = new ReplacementsPanel(top_frame, owning_controller);
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
                          &ActivitiesPanel::selectionChanged, (ActivitiesPanel *)parent);
}

void Activity::selectionChanged(wxCommandEvent &event) {
  ((ActivitiesPanel *)parent)->selectionChanged(event);
}

ActivitiesPanel::ActivitiesPanel(wxWindow *parent,
                                 ScreenTextController *owning_controller)
    : wxPanel(parent) {
  this->owning_controller = owning_controller;
  this->parent = parent;
  activities.push_back(Activity(this, parent, owning_controller, true));
  bindEvents();
  positionWidgets();
}

void ActivitiesPanel::addActivity(wxPanel *parent_panel) {
  bool is_first = (activities.empty());
  activities.push_back(
      Activity(this, parent_panel, owning_controller, is_first));
  for (auto widget : activities.back().line()) {
    GetSizer()->Add(widget, 0, wxALL, BORDER_SIZE);
  }
  parent_panel->GetSizer()->Add(activities.back().replacementsPanel(), 0, wxALL,
                                BORDER_SIZE);
  activities.back().select();
  selectionChanged(wxCommandEvent());
  SetSizerAndFit(GetSizer());
}

void ActivitiesPanel::selectionChanged(wxCommandEvent &event) {
  for (auto activity : activities) {
    if (activity.isSelected()) {
      activity.replacementsPanel()->Show();
    } else {
      activity.replacementsPanel()->Hide();
    }
  }
  owning_controller->updatePreview();
}

void ActivitiesPanel::bindEvents() {}

ReplacementsPanel *ActivitiesPanel::replacementsPanel() {
  if (activities.empty()) {
    return nullptr;
  }

  for (auto activity : activities) {
    if (activity.isSelected()) {
      return activity.replacementsPanel();
    }
  }

  // Should be unreachable.
  return activities[0].replacementsPanel();
}

void ActivitiesPanel::positionWidgets() {
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
