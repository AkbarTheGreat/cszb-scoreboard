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
const int NUMBER_OF_PRESENTER_OPTIONS = 2;
const wxString PRESENTER_OPTIONS[NUMBER_OF_PRESENTER_OPTIONS] = {
    wxT("Activity List"), wxT("Replacements")};

ThingsMode *ThingsMode::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  ThingsMode *entry = new ThingsMode(preview_panel, parent);
  entry->initializeWidgets();
  return entry;
}

void ThingsMode::createControls(wxPanel *control_panel) {
  scrollable_panel = new wxScrolledWindow(
      control_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);

  button_panel = new wxPanel(scrollable_panel);

  screen_selection = new TeamSelector(button_panel);
  presenter_selection = new wxRadioBox(
      button_panel, wxID_ANY, wxT("Present"), wxDefaultPosition, wxDefaultSize,
      NUMBER_OF_PRESENTER_OPTIONS, PRESENTER_OPTIONS, 1, wxRA_SPECIFY_COLS);
  presenter_selection->SetSelection(0);

  new_activity_button = new wxButton(button_panel, wxID_ANY, "New Activity");
  new_replacement_button =
      new wxButton(button_panel, wxID_ANY, "New Replacement");

  home_activities_panel = new ActivityPanel(scrollable_panel, this);
  away_activities_panel = new ActivityPanel(scrollable_panel, this);
  all_activities_panel = new ActivityPanel(scrollable_panel, this);

  positionWidgets(control_panel);
  bindEvents();
}  // namespace cszb_scoreboard

void ThingsMode::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *button_sizer = new wxFlexGridSizer(0, 2, 0, 0);
  button_sizer->SetFlexibleDirection(wxBOTH);
  button_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  button_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);
  button_sizer->Add(presenter_selection, 0, wxALL, BORDER_SIZE);
  button_sizer->Add(new_activity_button, 0, wxALL, BORDER_SIZE);
  button_sizer->Add(new_replacement_button, 0, wxALL, BORDER_SIZE);
  button_panel->SetSizerAndFit(button_sizer);

  wxFlexGridSizer *outer_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  outer_sizer->SetFlexibleDirection(wxBOTH);
  outer_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *scrollable_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  scrollable_sizer->SetFlexibleDirection(wxBOTH);
  scrollable_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  scrollable_sizer->Add(button_panel, 0, wxALL, BORDER_SIZE);

  scrollable_sizer->Add(home_activities_panel);
  scrollable_sizer->Add(away_activities_panel);
  scrollable_sizer->Add(all_activities_panel);

  updateActivityPanel();

  scrollable_panel->SetSizer(scrollable_sizer);
  scrollable_panel->FitInside();
  scrollable_panel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);

  outer_sizer->Add(scrollable_panel, 0, wxALL, BORDER_SIZE);
  control_panel->SetSizerAndFit(outer_sizer);
}

void ThingsMode::bindEvents() {
  new_activity_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                            &ThingsMode::addActivity, this);
  new_replacement_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                               &ThingsMode::addReplacement, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &ThingsMode::screenChanged, this);
  presenter_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                            &ThingsMode::presentedListChanged, this);
}

void ThingsMode::updatePreview() {
  // Re-size for scrollable windows
  scrollable_panel->SetSizer(scrollable_panel->GetSizer());
  scrollable_panel->FitInside();
  scrollable_panel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
  scrollable_panel->SetScrollRate(0, 20);

  // Set all sides to true, in order to present to all monitors
  proto::ScreenSide side;
  side.set_home(true);
  side.set_away(true);
  side.set_extra(true);

  // TODO: Centralize team colors and use that here instead
  Color screen_color("Black");
  ActivityPanel *selected_panel = all_activities_panel;
  if (screen_selection->allSelected()) {
    // Do nothing, these are already set.
  } else if (screen_selection->homeSelected()) {
    screen_color = Color("Blue");
    selected_panel = home_activities_panel;
  } else if (screen_selection->awaySelected()) {
    screen_color = Color("Red");
    selected_panel = away_activities_panel;
  }

  std::vector<proto::RenderableText> screen_lines;

  if (presenter_selection->GetSelection() == 0) {
    screen_lines.push_back(proto::RenderableText());
    screen_lines.back().set_text("Activities Go Here");
    screen_lines.back().mutable_font()->set_size(DEFAULT_FONT_SIZE);
  } else {
    screen_lines.push_back(proto::RenderableText());
    screen_lines.back().set_text("Replacements Go Here");
    screen_lines.back().mutable_font()->set_size(DEFAULT_FONT_SIZE);
  }

  previewPanel()->setTextForPreview(screen_lines, screen_color, true, side);
}

void ThingsMode::textUpdated(wxKeyEvent &event) { updatePreview(); }

void ThingsMode::updateActivityPanel() {
  if (screen_selection->allSelected()) {
    home_activities_panel->Hide();
    away_activities_panel->Hide();
    all_activities_panel->Show();
  } else if (screen_selection->homeSelected()) {
    away_activities_panel->Hide();
    all_activities_panel->Hide();
    home_activities_panel->Show();
  } else if (screen_selection->awaySelected()) {
    home_activities_panel->Hide();
    all_activities_panel->Hide();
    away_activities_panel->Show();
  }
}

void ThingsMode::screenChanged(wxCommandEvent &event) {
  updateActivityPanel();
  updatePreview();
}

void ThingsMode::presentedListChanged(wxCommandEvent &event) {
  updatePreview();
}

void ThingsMode::addActivity(wxCommandEvent &event) {
  if (screen_selection->allSelected()) {
    all_activities_panel->addActivity(scrollable_panel);
  } else if (screen_selection->homeSelected()) {
    home_activities_panel->addActivity(scrollable_panel);
  } else if (screen_selection->awaySelected()) {
    away_activities_panel->addActivity(scrollable_panel);
  }
}

void ThingsMode::addReplacement(wxCommandEvent &event) {
  if (screen_selection->allSelected()) {
    all_activities_panel->addReplacement();
  } else if (screen_selection->homeSelected()) {
    home_activities_panel->addReplacement();
  } else if (screen_selection->awaySelected()) {
    away_activities_panel->addReplacement();
  }
}

}  // namespace cszb_scoreboard
