/*
ui/dialog/settings/TeamSettingsPanel.cpp: The panel which contains team
settings.

Copyright 2019-2023 Tracy Beck

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

#include <string>  // for string

#include "ScoreboardCommon.h"                     // for DEFAULT_BORDER_SIZE
#include "config/TeamConfig.h"                    // for TeamConfig
#include "config/swx/event.h"                     // for wxEVT_COMMAND_BUTTO...
#include "ui/dialog/settings/TeamSettingsPage.h"  // for TeamSettingsPage
#include "ui/graphics/Color.h"                    // for Color

namespace cszb_scoreboard {
namespace proto {
enum TeamInfo_TeamType : int;
}  // namespace proto

namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const std::string ARROW_TOOL_TIP =
    "Change the order of teams appear on the same screen and in score "
    "controls.  Requires a restart to take effect.";

TeamSettingsPanel::TeamSettingsPanel(swx::Panel *wx, int team_index,
                                     proto::TeamInfo_TeamType team,
                                     TeamSettingsPage *parent,
                                     Singleton *singleton)
    : Panel(wx) {
  this->parent = parent;
  index = team_index;
  team_type = team;

  // Label for this display
  team_label = label(TeamConfig::teamName(team_type));
  team_label->bold(true);
  addWidget(*team_label, 0, 0);

  createButtonPanel();
  if (index == 0) {
    up_button->disable();
  }
  if (index >= singleton->teamConfig()->numberOfTeams() - 1) {
    down_button->disable();
  }
  addWidget(*button_panel, 0, 1);

  default_color_label = label("Default Color");
  addWidget(*default_color_label, 1, 0);
  color_picker = colorPicker(singleton->teamConfig()->teamColor(team));
  addWidget(*color_picker, 1, 1);

  runSizer();
}

void TeamSettingsPanel::copyFrom(const TeamSettingsPanel &other) {
  team_label->set(other.team_label->text());
  color_picker->setColor(other.color_picker->color());
  team_type = other.team_type;
}

void TeamSettingsPanel::createButtonPanel() {
  button_panel = panel();
  up_button = button_panel->button("^", true);
  down_button = button_panel->button("v", true);

  button_panel->addWidget(*up_button, 0, 0);
  button_panel->addWidget(*down_button, 0, 1);

  up_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->moveTeam(true); });
  down_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->moveTeam(false); });

  up_button->toolTip(ARROW_TOOL_TIP);
  down_button->toolTip(ARROW_TOOL_TIP);

  button_panel->runSizer();
}

auto TeamSettingsPanel::teamColor() -> Color { return color_picker->color(); }

void TeamSettingsPanel::moveTeam(bool is_up_button) {
  if (is_up_button) {
    parent->swapTeams(index, index - 1);
  } else {
    parent->swapTeams(index, index + 1);
  }
}

}  // namespace cszb_scoreboard
