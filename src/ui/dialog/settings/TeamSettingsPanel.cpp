/*
ui/dialog/settings/TeamSettingsPanel.cpp: The panel which contains team
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

#include "ui/dialog/settings/TeamSettingsPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

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
  sizer->Add(label, 0, wxALL, BORDER_SIZE);
  // Consume the right column on this row
  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("")), 0, wxALL, BORDER_SIZE);

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Default Color")), 0, wxALL,
             BORDER_SIZE);
  color_picker = new wxColourPickerCtrl(
      this, wxID_ANY, TeamConfig::getInstance()->teamColor(team_index));
  sizer->Add(color_picker, 0, wxALL, BORDER_SIZE);

  SetSizerAndFit(sizer);
}

Color TeamSettingsPanel::teamColor() {
  wxColour wx_color = color_picker->GetColour();
  return Color(wx_color);
}

}  // namespace cszb_scoreboard
