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
#include "util/StringUtil.h"

#include <config/DisplayConfig.h>
#include <config/TeamConfig.h>
#include <wx/bookctrl.h>

namespace cszb_scoreboard {

wxDEFINE_EVENT(SETTINGS_UPDATED, wxCommandEvent);

bool SettingsDialog::Create(wxWindow* parent) {
  this->parent = parent;
  if (!wxPropertySheetDialog::Create(parent, wxID_ANY, "Scoreboard Settings")) {
    return false;
  }
  CreateButtons(wxOK | wxCANCEL);
  wxBookCtrlBase* settings_book = GetBookCtrl();
  settings_book->AddPage(createTeamsPage(settings_book), "Teams");
  settings_book->AddPage(createDisplayPage(settings_book), "Displays");
  LayoutDialog();
  bindEvents();
  return true;
}

wxPanel* SettingsDialog::createTeamsPage(wxBookCtrlBase* settings_book) {
  wxPanel* panel = new wxPanel(settings_book);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  for (int i = 0; i < TeamConfig::getInstance()->numberOfTeams(); ++i) {
    TeamSettingsPanel* team_panel = new TeamSettingsPanel(panel, i);
    team_settings_panels.push_back(team_panel);
    sizer->Add(team_panel, 0, wxALL, 5);
  }

  panel->SetSizerAndFit(sizer);
  return panel;
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
  Bind(wxEVT_BUTTON, &SettingsDialog::onCancel, this, wxID_CANCEL);
  Bind(wxEVT_CLOSE_WINDOW, &SettingsDialog::onClose, this);
}

void SettingsDialog::onOk(wxCommandEvent& event) {
  if (validateSettings()) {
    saveSettings();
    wxCommandEvent settings_updated_event(SETTINGS_UPDATED);
    // Normally, I'd use wxPostEvent here for asynchronous event handling, but
    // it sometimes doesn't work (Maybe due to a race condition with object
    // destruction?)  Processing synchronously here is our best compromise.
    ProcessEvent(settings_updated_event);
    Close(true);
    return;
  }
}

void SettingsDialog::onCancel(wxCommandEvent& event) { Close(true); }

void SettingsDialog::onClose(wxCloseEvent& event) {
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog.  To top that off, if you set focus
  // before calling Destroy(), things quit working.  But Destroying calls the
  // destructor, so we can't rely on this->parent anymore after Destroy is
  // called.  So we save it in a local pointer temporarily for this purpose.
  wxWindow* local_parent = parent;
  Destroy();
  local_parent->SetFocus();
}

bool SettingsDialog::validateSettings() {
  if (!validateDisplaySettings()) {
    return false;
  }
  if (!validateTeamSettings()) {
    return false;
  }
  return true;
}

void SettingsDialog::saveSettings() {
  saveDisplaySettings();
  saveTeamSettings();
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

bool SettingsDialog::validateTeamSettings() {
  // At present, nothing to validate
  return true;
}

void SettingsDialog::saveTeamSettings() {
  for (int i = 0; i < team_settings_panels.size(); ++i) {
    TeamConfig::getInstance()->setColor(i,
                                        team_settings_panels[i]->teamColor());
  }
  TeamConfig::getInstance()->saveSettings();
}

DisplaySettingsPanel::DisplaySettingsPanel(wxPanel* parent, int display_number)
    : wxPanel(parent) {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(display_number);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);

  proto::ScreenSide screen_side = display_info.side();
  // Label for this display
  wxString label_text = wxT("Display ");
  label_text += StringUtil::intToString(display_number+1);
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

TeamSettingsPanel::TeamSettingsPanel(wxPanel* parent, int team_index)
    : wxPanel(parent) {
  proto::TeamInfo team_info = TeamConfig::getInstance()->teamInfo(team_index);

  wxGridSizer* sizer = new wxGridSizer(0, 2, 0, 0);

  // Label for this display
  wxStaticText* label = new wxStaticText(
      this, wxID_ANY, TeamConfig::getInstance()->teamName(team_index));
  wxFont font = label->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  label->SetFont(font);
  sizer->Add(label, 0, wxALL, 5);
  // Consume the right column on this row
  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("")), 0, wxALL, 5);

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Default Color")), 0, wxALL,
             5);
  color_picker = new wxColourPickerCtrl(
      this, wxID_ANY, TeamConfig::getInstance()->teamColor(team_index));
  sizer->Add(color_picker, 0, wxALL, 5);

  SetSizerAndFit(sizer);
}

Color TeamSettingsPanel::teamColor() {
  wxColour wx_color = color_picker->GetColour();
  return Color(wx_color);
}

}  // namespace cszb_scoreboard
