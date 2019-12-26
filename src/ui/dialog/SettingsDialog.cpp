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
  if (!wxPropertySheetDialog::Create(parent, wxID_ANY, "Scoreboard Settings")) {
    return false;
  }
  CreateButtons(wxOK | wxCANCEL);
  wxBookCtrlBase* settings_book = GetBookCtrl();
  settings_book->AddPage(createDisplayPage(settings_book), "Displays");
  LayoutDialog();
  return true;
}

wxPanel* SettingsDialog::createDisplayPage(wxBookCtrlBase* settings_book) {
  wxPanel* panel = new wxPanel(settings_book);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    proto::ScreenSide screen_side = display_info.side();
    // Label for this display
    char* string_buffer = new char[16];
    wxString label_text = wxT("Display ");
    label_text += _itoa(i + 1, string_buffer, 10);
    wxStaticText* label = new wxStaticText(panel, wxID_ANY, label_text);
    sizer->Add(label, 0, wxALL, 5);
    // Booth monitor checkbox
    wxCheckBox* control_checkbox =
        new wxCheckBox(panel, wxID_ANY, wxT("&Booth Monitor"));
    control_checkbox->SetValue(screen_side.control());
    sizer->Add(control_checkbox, 0, wxALL, 5);
    // Home team checkbox
    wxCheckBox* home_checkbox =
        new wxCheckBox(panel, wxID_ANY, wxT("&Home Team"));
    home_checkbox->SetValue(screen_side.home());
    sizer->Add(home_checkbox, 0, wxALL, 5);
    // Away team checkbox
    wxCheckBox* away_checkbox =
        new wxCheckBox(panel, wxID_ANY, wxT("&Away Team"));
    away_checkbox->SetValue(screen_side.away());
    sizer->Add(away_checkbox, 0, wxALL, 5);
  }

  panel->SetSizerAndFit(sizer);
  return panel;
}

}  // namespace cszb_scoreboard
