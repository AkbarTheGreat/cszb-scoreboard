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
  new_replacement_button->Disable();

  home_activities_panel = new wxPanel(scrollable_panel);
  home_replacements_panel = new wxPanel(scrollable_panel);
  away_activities_panel = new wxPanel(scrollable_panel);
  away_replacements_panel = new wxPanel(scrollable_panel);
  all_activities_panel = new wxPanel(scrollable_panel);
  all_replacements_panel = new wxPanel(scrollable_panel);

  positionWidgets(control_panel);
  bindEvents();
}

void ThingsMode::placeAndSizePanel(int rows, wxPanel *panel) {
  wxFlexGridSizer *panel_sizer = new wxFlexGridSizer(0, rows, 0, 0);
  panel_sizer->SetFlexibleDirection(wxBOTH);
  panel_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  panel->SetSizerAndFit(panel_sizer);
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

  placeAndSizePanel(3, home_activities_panel);
  scrollable_sizer->Add(home_activities_panel);
  placeAndSizePanel(3, home_replacements_panel);
  scrollable_sizer->Add(home_replacements_panel);

  placeAndSizePanel(3, away_activities_panel);
  scrollable_sizer->Add(away_activities_panel);
  placeAndSizePanel(3, away_replacements_panel);
  scrollable_sizer->Add(away_replacements_panel);

  placeAndSizePanel(3, all_activities_panel);
  scrollable_sizer->Add(all_activities_panel);
  placeAndSizePanel(3, all_replacements_panel);
  scrollable_sizer->Add(all_replacements_panel);

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
    home_replacements_panel->Hide();
    away_activities_panel->Hide();
    away_replacements_panel->Hide();
    all_activities_panel->Show();
    all_replacements_panel->Show();
  } else if (screen_selection->homeSelected()) {
    away_activities_panel->Hide();
    away_replacements_panel->Hide();
    all_activities_panel->Hide();
    all_replacements_panel->Hide();
    home_activities_panel->Show();
    home_replacements_panel->Show();
  } else if (screen_selection->awaySelected()) {
    home_activities_panel->Hide();
    home_replacements_panel->Hide();
    all_activities_panel->Hide();
    all_replacements_panel->Hide();
    away_activities_panel->Show();
    away_replacements_panel->Show();
  }

  updatePreview();
}

void ThingsMode::addActivity(wxCommandEvent &event) {
  wxPanel *panel = home_activities_panel;

  wxSizer *sizer = panel->GetSizer();
  wxRadioButton *radio = new wxRadioButton(panel, wxID_ANY, "");
  wxTextCtrl *entry = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition,
                                     wxSize(-1, -1), wxTE_MULTILINE);
  wxButton *delete_button = new wxButton(
      panel, wxID_ANY, "X", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  sizer->Add(radio, 0, wxALL, BORDER_SIZE);
  sizer->Add(entry, 0, wxALL, BORDER_SIZE);
  sizer->Add(delete_button, 0, wxALL, BORDER_SIZE);
  panel->SetSizerAndFit(sizer);

  scrollable_panel->SetSizer(scrollable_panel->GetSizer());
  scrollable_panel->FitInside();
  scrollable_panel->SetScrollRate(0, scrollable_panel->GetSize().GetHeight());
  scrollable_panel->SetSizeHints(scrollable_panel->GetSize());
  //  scrollable_panel->SetSize(scrollable_panel->GetVirtualSize().GetWidth(),
  //                            scrollable_panel->GetSize().GetHeight());

  control_panel->SetSizerAndFit(control_panel->GetSizer());

  home_activity_selectors.push_back(radio);
  home_activities.push_back(entry);
  home_remove_activity_buttons.push_back(delete_button);
}

void ThingsMode::addReplacement(wxCommandEvent &event) {}

}  // namespace cszb_scoreboard
