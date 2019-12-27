/*
ui/dialog/SettingsDialog.cpp: A settings dialog for configuring various aspects
of the scoreboard which are then saved as a part of the configuration protobuf.

Copyright 2019 Tracy Beck

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

#include "ui/dialog/SettingsDialog.h"

#include <config/DisplayConfig.h>
#include <wx/bookctrl.h>

namespace cszb_scoreboard {

bool SettingsDialog::Create(wxWindow* parent) {
  this->parent = parent;
  if (!wxPropertySheetDialog::Create(parent, wxID_ANY, "Scoreboard Settings")) {
    return false;
  }
  CreateButtons(wxOK | wxCANCEL);
  wxBookCtrlBase* settings_book = GetBookCtrl();
  settings_book->AddPage(createDisplayPage(settings_book), "Displays");
  LayoutDialog();
  bindEvents();
  return true;
}

wxPanel* SettingsDialog::createDisplayPage(wxBookCtrlBase* settings_book) {
  wxPanel* panel = new wxPanel(settings_book);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    DisplaySettingsPanel* display_panel = new DisplaySettingsPanel(panel, i);
    display_settings_panels.push_back(display_panel);
    sizer->Add(display_panel, 0, wxALL, 5);
  }

  panel->SetSizerAndFit(sizer);
  return panel;
}

void SettingsDialog::bindEvents() {
  Bind(wxEVT_BUTTON, &SettingsDialog::onOk, this, wxID_OK);
}

void SettingsDialog::onOk(wxCommandEvent& event) {
  if (validateDisplaySettings()) {
    parent->SetFocus();
    saveDisplaySettings();
    Close(true);
    return;
  }
}

/* Returns true if the display settings are allowable, presents a warning dialog
 * if not (and returns false). */
bool SettingsDialog::validateDisplaySettings() {
  bool has_control = false;
  for (auto display_panel : display_settings_panels) {
    proto::ScreenSide side = display_panel->getSide();
    if (side.control()) {
      // Two controls, that's problematic.
      if (has_control) {
        wxMessageBox(
            "ERROR: Only one window may be selected as a Booth Monitor.");
        return false;
      }
      has_control = true;
      if (side.home() || side.away() || side.extra()) {
// This can't be checked in debug, because we break this rule often in debug
// mode.
#ifndef WXDEBUG
        wxMessageBox(
            "ERROR: The Booth Monitor display may not also be a team display.");
        return false;
#endif
      }
    }
  }
  if (!has_control) {
    wxMessageBox("ERROR: One window must be selected as a Booth Monitor.");
    return false;
  }
  return true;
}

void SettingsDialog::saveDisplaySettings() {
  for (int i = 0; i < display_settings_panels.size(); ++i) {
    DisplayConfig::getInstance()->setSide(
        i, display_settings_panels[i]->getSide());
  }
  DisplayConfig::getInstance()->saveSettings();
}

DisplaySettingsPanel::DisplaySettingsPanel(wxPanel* parent, int display_number)
    : wxPanel(parent) {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(display_number);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);

  proto::ScreenSide screen_side = display_info.side();
  // Label for this display
  char* string_buffer = new char[16];
  wxString label_text = wxT("Display ");
  label_text += _itoa(display_number + 1, string_buffer, 10);
  wxStaticText* label = new wxStaticText(this, wxID_ANY, label_text);
  wxFont font = label->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  label->SetFont(font);
  sizer->Add(label, 0, wxALL, 5);

  // Booth monitor checkbox
  control_checkbox = new wxCheckBox(this, wxID_ANY, wxT("&Booth Monitor"));
  control_checkbox->SetValue(screen_side.control());
  sizer->Add(control_checkbox, 0, wxALL, 5);

  // Home team checkbox
  home_checkbox = new wxCheckBox(this, wxID_ANY, wxT("&Home Team"));
  home_checkbox->SetValue(screen_side.home());
  sizer->Add(home_checkbox, 0, wxALL, 5);

  // Away team checkbox
  away_checkbox = new wxCheckBox(this, wxID_ANY, wxT("&Away Team"));
  away_checkbox->SetValue(screen_side.away());
  sizer->Add(away_checkbox, 0, wxALL, 5);

  SetSizerAndFit(sizer);
}

proto::ScreenSide DisplaySettingsPanel::getSide() {
  proto::ScreenSide side;
  side.set_control(control_checkbox->GetValue());
  side.set_home(home_checkbox->GetValue());
  side.set_away(away_checkbox->GetValue());
  return side;
}

}  // namespace cszb_scoreboard
