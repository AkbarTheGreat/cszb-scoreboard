/*
ui/dialog/settings/DisplaySettingsPanel.cpp: The panel which contains display
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

#include "ui/dialog/settings/DisplaySettingsPanel.h"

#include "ui/UiUtil.h"
#include "ui/dialog/settings/DisplaySettingsPage.h"
#include "util/Log.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const std::string ARROW_TOOL_TIP =
    "Change the order of displays.  This primarily affects the order used to "
    "show previews on the main screen.  Requires a restart to take effect.";

DisplaySettingsPanel::DisplaySettingsPanel(swx::Panel* wx, int index,
                                           DisplaySettingsPage* parent)
    : Panel(wx) {
  this->index = index;
  this->parent = parent;
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(index);
  this->display_id = display_info.id();

  const proto::ScreenSide& screen_side = display_info.side();
  // Label for this display
  display_label = new wxStaticText(wx, wxID_ANY, "");
  wxFont font = display_label->GetFont();
  font.SetWeight(wxFONTWEIGHT_BOLD);
  display_label->SetFont(font);
  updateLabel();
  UiUtil::addToGridBag(sizer(), display_label, 0, 0);

  // Up/Down buttons
  createButtonPanel();
  if (index == 0) {
    up_button->Disable();
  }
  if (index >= DisplayConfig::getInstance()->numberOfDisplays() - 1) {
    down_button->Disable();
  }

  UiUtil::addToGridBag(sizer(), button_panel, 1, 0);

  // Booth monitor checkbox
  control_checkbox = new wxCheckBox(wx, wxID_ANY, wxT("&Booth Monitor"));
  control_checkbox->SetValue(screen_side.control());
  UiUtil::addToGridBag(sizer(), control_checkbox, 2, 0);

  // Home team checkbox
  home_checkbox = new wxCheckBox(wx, wxID_ANY, wxT("&Home Team"));
  home_checkbox->SetValue(screen_side.home());
  UiUtil::addToGridBag(sizer(), home_checkbox, 3, 0);

  // Away team checkbox
  away_checkbox = new wxCheckBox(wx, wxID_ANY, wxT("&Away Team"));
  away_checkbox->SetValue(screen_side.away());
  UiUtil::addToGridBag(sizer(), away_checkbox, 4, 0);

  runSizer();
}

void DisplaySettingsPanel::copyFrom(const DisplaySettingsPanel& other) {
  this->display_id = other.display_id;
  copyCheckbox(other.control_checkbox, this->control_checkbox);
  copyCheckbox(other.home_checkbox, this->home_checkbox);
  copyCheckbox(other.away_checkbox, this->away_checkbox);
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

  button_panel = new wxPanel(wx);
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
  if (event.GetEventObject() == up_button) {
    parent->swapDisplays(index, index - 1);
  } else if (event.GetEventObject() == down_button) {
    parent->swapDisplays(index, index + 1);
  } else {
    LogDebug("Button clicked, but not the up or down button.  That's weird.");
  }
}

}  // namespace cszb_scoreboard
