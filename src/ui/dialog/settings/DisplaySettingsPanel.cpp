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

#include "ui/UiUtil.h"
#include "ui/dialog/settings/DisplaySettingsPage.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const std::string ARROW_TOOL_TIP =
    "Change the order of displays.  This primarily affects the order used to "
    "show previews on the main screen.  Requires a restart to take effect.";

DisplaySettingsPanel::DisplaySettingsPanel(wxPanel* parent, int index)
    : wxPanel(parent) {
  this->index = index;
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(index);
  this->display_id = display_info.id();
  wxSizer* sizer = UiUtil::sizer(0, 1);

  const proto::ScreenSide& screen_side = display_info.side();
  // Label for this display
  display_label = new wxStaticText(this, wxID_ANY, "");
  wxFont font = display_label->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  display_label->SetFont(font);
  updateLabel();
  sizer->Add(display_label, 0, wxALL, BORDER_SIZE);

  // Up/Down buttons
  createButtonPanel();
  if (index == 0) {
    up_button->Disable();
  }
  if (index >= DisplayConfig::getInstance()->numberOfDisplays() - 1) {
    down_button->Disable();
  }

  sizer->Add(button_panel, 0, wxALL, BORDER_SIZE);

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

void DisplaySettingsPanel::copyFrom(DisplaySettingsPanel* other) {
  this->display_id = other->display_id;
  copyCheckbox(other->control_checkbox, this->control_checkbox);
  copyCheckbox(other->home_checkbox, this->home_checkbox);
  copyCheckbox(other->away_checkbox, this->away_checkbox);
  updateLabel();
}

void DisplaySettingsPanel::copyCheckbox(wxCheckBox* source,
                                        wxCheckBox* target) {
  target->SetValue(source->GetValue());
}

void DisplaySettingsPanel::updateLabel() {
  wxString label_text = wxT("Display ");
  label_text += StringUtil::intToString(display_id + 1);
  display_label->SetLabel(label_text);
}

void DisplaySettingsPanel::createButtonPanel() {
  wxSizer* sizer = UiUtil::sizer(0, 2);

  button_panel = new wxPanel(this);
  up_button = new wxButton(button_panel, wxID_ANY, "^", wxDefaultPosition,
                           wxDefaultSize, wxBU_EXACTFIT);
  down_button = new wxButton(button_panel, wxID_ANY, "v", wxDefaultPosition,
                             wxDefaultSize, wxBU_EXACTFIT);

  sizer->Add(up_button, 0, wxALL, BORDER_SIZE);
  sizer->Add(down_button, 0, wxALL, BORDER_SIZE);

  up_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                  &DisplaySettingsPanel::moveDisplay, this);
  down_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                    &DisplaySettingsPanel::moveDisplay, this);

  up_button->SetToolTip(ARROW_TOOL_TIP);
  down_button->SetToolTip(ARROW_TOOL_TIP);

  button_panel->SetSizerAndFit(sizer);
}

auto DisplaySettingsPanel::getSide() -> proto::ScreenSide {
  proto::ScreenSide side;
  side.set_control(control_checkbox->GetValue());
  side.set_home(home_checkbox->GetValue());
  side.set_away(away_checkbox->GetValue());
  return side;
}

void DisplaySettingsPanel::moveDisplay(wxCommandEvent& event) {
  auto* parent_page = dynamic_cast<DisplaySettingsPage*>(GetParent());
  if (event.GetEventObject() == up_button) {
    parent_page->swapDisplays(index, index - 1);
  } else if (event.GetEventObject() == down_button) {
    parent_page->swapDisplays(index, index + 1);
  } else {
    wxLogDebug("Button clicked, but not the up or down button.  That's weird.");
  }
}

}  // namespace cszb_scoreboard
