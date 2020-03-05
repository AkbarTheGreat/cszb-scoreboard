/*
ui/dialog/settings/DisplaySettingsPanel.cpp: The panel which contains display
settings.

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

#include "ui/dialog/settings/DisplaySettingsPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

DisplaySettingsPanel::DisplaySettingsPanel(wxPanel* parent, int display_number)
    : wxPanel(parent) {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(display_number);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);

  proto::ScreenSide screen_side = display_info.side();
  // Label for this display
  wxString label_text = wxT("Display ");
  label_text += StringUtil::intToString(display_number + 1);
  wxStaticText* label = new wxStaticText(this, wxID_ANY, label_text);
  wxFont font = label->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  label->SetFont(font);
  sizer->Add(label, 0, wxALL, BORDER_SIZE);

  // Booth monitor checkbox
  control_checkbox = new wxCheckBox(this, wxID_ANY, wxT("&Booth Monitor"));
  control_checkbox->SetValue(screen_side.control());
  sizer->Add(control_checkbox, 0, wxALL, BORDER_SIZE);

  // Home team checkbox
  home_checkbox = new wxCheckBox(this, wxID_ANY, wxT("&Home Team"));
  home_checkbox->SetValue(screen_side.home());
  sizer->Add(home_checkbox, 0, wxALL, BORDER_SIZE);

  // Away team checkbox
  away_checkbox = new wxCheckBox(this, wxID_ANY, wxT("&Away Team"));
  away_checkbox->SetValue(screen_side.away());
  sizer->Add(away_checkbox, 0, wxALL, BORDER_SIZE);

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
