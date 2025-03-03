/*
ui/dialog/settings/DisplaySettingsPanel.cpp: The panel which contains display
settings.

Copyright 2019-2025 Tracy Beck

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

#include <string>

#include "ScoreboardCommon.h"
#include "config/DisplayConfig.h"
#include "config/swx/event.h"
#include "ui/dialog/settings/DisplaySettingsPage.h"

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const std::string ARROW_TOOL_TIP =
    "Change the order of displays.  This primarily affects the order used to "
    "show previews on the main screen.  Requires a restart to take effect.";

DisplaySettingsPanel::DisplaySettingsPanel(swx::Panel *wx, int index,
                                           DisplaySettingsPage *parent,
                                           Singleton *singleton)
    : Panel(wx) {
  this->index = index;
  this->parent = parent;
  proto::DisplayInfo display_info =
      singleton->displayConfig()->displayDetails(index);
  this->display_id = display_info.id();
  this->display_size = buildDisplaySize(display_info.dimensions());

  const proto::ScreenSide &screen_side = display_info.side();
  // Label for this display
  display_label = label("");
  display_label->bold(true);
  updateLabel();
  addWidget(*display_label, 0, 0);

  // Up/Down buttons
  createButtonPanel();
  if (index == 0) {
    up_button->disable();
  }
  if (index >= singleton->displayConfig()->numberOfDisplays() - 1) {
    down_button->disable();
  }

  addWidget(*button_panel, 1, 0);

  // Booth monitor checkbox
  control_checkbox = checkBox("&Booth Monitor");
  control_checkbox->setChecked(screen_side.control());
  addWidget(*control_checkbox, 2, 0);

  // Home team checkbox
  home_checkbox = checkBox("&Home Team");
  home_checkbox->setChecked(screen_side.home());
  addWidget(*home_checkbox, 3, 0);

  // Away team checkbox
  away_checkbox = checkBox("&Away Team");
  away_checkbox->setChecked(screen_side.away());
  addWidget(*away_checkbox, 4, 0);

  runSizer();
}

void DisplaySettingsPanel::copyFrom(const DisplaySettingsPanel &other) {
  this->display_id = other.display_id;
  this->display_size = other.display_size;
  copyCheckbox(*other.control_checkbox, this->control_checkbox.get());
  copyCheckbox(*other.home_checkbox, this->home_checkbox.get());
  copyCheckbox(*other.away_checkbox, this->away_checkbox.get());
  updateLabel();
}

auto DisplaySettingsPanel::buildDisplaySize(const proto::Rectangle &dimensions)
    -> std::string {
  return std::to_string(dimensions.height()) + "x" +
         std::to_string(dimensions.width());
}

void DisplaySettingsPanel::copyCheckbox(const CheckBox &source,
                                        CheckBox *target) {
  target->setChecked(source.checked());
}

void DisplaySettingsPanel::updateLabel() {
  display_label->set("Display " + std::to_string(display_id + 1) + " (" +
                     display_size + ")");
}

void DisplaySettingsPanel::createButtonPanel() {
  button_panel = panel();
  up_button = button_panel->button("^", true);
  down_button = button_panel->button("v", true);

  button_panel->addWidget(*up_button, 0, 0);
  button_panel->addWidget(*down_button, 0, 1);

  up_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->moveDisplay(true); });
  down_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->moveDisplay(false); });

  up_button->toolTip(ARROW_TOOL_TIP);
  down_button->toolTip(ARROW_TOOL_TIP);

  button_panel->runSizer();
}

auto DisplaySettingsPanel::getSide() -> proto::ScreenSide {
  proto::ScreenSide side;
  side.set_control(control_checkbox->checked());
  side.set_home(home_checkbox->checked());
  side.set_away(away_checkbox->checked());
  return side;
}

void DisplaySettingsPanel::moveDisplay(bool is_up_button) {
  if (is_up_button) {
    parent->swapDisplays(index, index - 1);
  } else {
    parent->swapDisplays(index, index + 1);
  }
}

}  // namespace cszb_scoreboard
