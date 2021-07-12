/*
ui/dialog/settings/DisplaySettingsPanel.h: The panel which contains display
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

#include <wx/wx.h>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "config/DisplayConfig.h"
#include "ui/widget/Label.h"
#include "ui/widget/Panel.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

// Pre-defining ActivityPanel for a parent pointer.
class DisplaySettingsPage;

class DisplaySettingsPanel : public Panel {
 public:
  DisplaySettingsPanel(swx::Panel *wx, int index, DisplaySettingsPage *parent);
  void copyFrom(const DisplaySettingsPanel &other);
  auto getSide() -> proto::ScreenSide;
  [[nodiscard]] auto getDisplayId() const -> int { return display_id; }

 private:
  static void copyCheckbox(const CheckBox &source, CheckBox *target);
  void createButtonPanel();
  void moveDisplay(bool is_up_button);
  void updateLabel();

  int display_id;
  int index;
  std::unique_ptr<CheckBox> control_checkbox, home_checkbox, away_checkbox;
  std::unique_ptr<Label> display_label;
  std::unique_ptr<Panel> button_panel;
  std::unique_ptr<Button> down_button, up_button;
  DisplaySettingsPage *parent;
};

}  // namespace cszb_scoreboard
