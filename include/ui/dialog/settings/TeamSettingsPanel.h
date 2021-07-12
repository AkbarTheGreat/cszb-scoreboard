/*
ui/dialog/settings/TeamSettingsPanel.h: The panel which contains team
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
#pragma once

#include <wx/clrpicker.h>
#include <wx/wx.h>
#include <memory>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "config/TeamConfig.h"
#include "ui/graphics/Color.h"
#include "ui/widget/Button.h"
#include "ui/widget/ColorPicker.h"
#include "ui/widget/Label.h"
#include "ui/widget/Panel.h"

namespace cszb_scoreboard {

// Pre-defining TeamSettingsPage for a parent pointer.
class TeamSettingsPage;
namespace swx {
class Panel;
}  // namespace swx

class TeamSettingsPanel : public Panel {
 public:
  TeamSettingsPanel(swx::Panel *wx, int team_index,
                    proto::TeamInfo_TeamType team, TeamSettingsPage *parent);
  void copyFrom(const TeamSettingsPanel &other);
  auto teamColor() -> Color;
  auto team() -> proto::TeamInfo_TeamType { return team_type; }

 private:
  int index;
  proto::TeamInfo_TeamType team_type;
  std::unique_ptr<Label> team_label, default_color_label;
  std::unique_ptr<ColorPicker> color_picker;
  std::unique_ptr<Panel> button_panel;
  std::unique_ptr<Button> down_button, up_button;
  TeamSettingsPage *parent;
  void createButtonPanel();
  void moveTeam(bool is_up_button);
};

}  // namespace cszb_scoreboard
