/*
ui/dialog/settings/TeamSettingsPanel.cpp: The panel which contains team
settings.

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

#include "ui/dialog/settings/TeamSettingsPanel.h"

#include "ui/UiUtil.h"
#include "ui/dialog/settings/TeamSettingsPage.h"
#include "util/Log.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const std::string ARROW_TOOL_TIP =
    "Change the order of teams appear on the same screen and in score "
    "controls.  Requires a restart to take effect.";

TeamSettingsPanel::TeamSettingsPanel(wxPanel* parent, int team_index,
                                     proto::TeamInfo_TeamType team)
    : wxPanel(parent) {
  index = team_index;
  team_type = team;

  auto* sizer = new wxGridSizer(0, 2, 0, 0);

  // Label for this display
  label = new wxStaticText(this, wxID_ANY,
                           TeamConfig::getInstance()->teamName(team_type));
  wxFont font = label->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  label->SetFont(font);
  sizer->Add(label, 0, wxALL, BORDER_SIZE);

  createButtonPanel();
  if (index == 0) {
    up_button->Disable();
  }
  if (index >= TeamConfig::getInstance()->numberOfTeams() - 1) {
    down_button->Disable();
  }
  sizer->Add(button_panel, 0, wxALL, BORDER_SIZE);

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Default Color")), 0, wxALL,
             BORDER_SIZE);
  color_picker = new wxColourPickerCtrl(
      this, wxID_ANY, TeamConfig::getInstance()->teamColor(team));
  sizer->Add(color_picker, 0, wxALL, BORDER_SIZE);

  SetSizerAndFit(sizer);
}

void TeamSettingsPanel::copyFrom(TeamSettingsPanel* other) {
  label->SetLabelText(other->label->GetLabelText());
  color_picker->SetColour(other->color_picker->GetColour());
  team_type = other->team_type;
}

void TeamSettingsPanel::createButtonPanel() {
  wxSizer* sizer = UiUtil::sizer(0, 2);

  button_panel = new wxPanel(this);
  up_button = new wxButton(button_panel, wxID_ANY, "^", wxDefaultPosition,
                           wxDefaultSize, wxBU_EXACTFIT);
  down_button = new wxButton(button_panel, wxID_ANY, "v", wxDefaultPosition,
                             wxDefaultSize, wxBU_EXACTFIT);

  sizer->Add(up_button, 0, wxALL, BORDER_SIZE);
  sizer->Add(down_button, 0, wxALL, BORDER_SIZE);

  up_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TeamSettingsPanel::moveTeam,
                  this);
  down_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TeamSettingsPanel::moveTeam,
                    this);

  up_button->SetToolTip(ARROW_TOOL_TIP);
  down_button->SetToolTip(ARROW_TOOL_TIP);

  button_panel->SetSizerAndFit(sizer);
}

auto TeamSettingsPanel::teamColor() -> Color {
  wxColour wx_color = color_picker->GetColour();
  return Color(wx_color);
}

void TeamSettingsPanel::moveTeam(wxCommandEvent& event) {
  auto* parent_page = dynamic_cast<TeamSettingsPage*>(GetParent());
  if (event.GetEventObject() == up_button) {
    parent_page->swapTeams(index, index - 1);
  } else if (event.GetEventObject() == down_button) {
    parent_page->swapTeams(index, index + 1);
  } else {
    LogDebug("Button clicked, but not the up or down button.  That's weird.");
  }
}

}  // namespace cszb_scoreboard
