/*
ui/dialog/settings/DisplaySettingsPanel.h: The panel which contains display
settings.

Copyright 2019-2024 Tracy Beck

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

#include <memory>  // for unique_ptr
#include <string>  // for string

#include "ScoreboardCommon.h"    // for PUBLIC_TEST_ONLY
#include "config.pb.h"           // for Rectangle (ptr only), ScreenSide
#include "ui/widget/Button.h"    // for Button
#include "ui/widget/CheckBox.h"  // for CheckBox
#include "ui/widget/Label.h"     // for Label
#include "ui/widget/Panel.h"     // for Panel
#include "util/Singleton.h"      // for Singleton

namespace cszb_scoreboard {

// Pre-defining DisplaySettingsPage for a parent pointer.
class DisplaySettingsPage;

namespace swx {
class Panel;
}  // namespace swx

class DisplaySettingsPanel : public Panel {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  DisplaySettingsPanel(swx::Panel *wx, int index, DisplaySettingsPage *parent)
      : DisplaySettingsPanel(wx, index, parent, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void copyFrom(const DisplaySettingsPanel &other);
  auto getSide() -> proto::ScreenSide;
  [[nodiscard]] auto getDisplayId() const -> int { return display_id; }

  PUBLIC_TEST_ONLY
  DisplaySettingsPanel(swx::Panel *wx, int index, DisplaySettingsPage *parent,
                       Singleton *singleton);

 private:
  static auto buildDisplaySize(const proto::Rectangle &dimensions)
      -> std::string;
  static void copyCheckbox(const CheckBox &source, CheckBox *target);
  void createButtonPanel();
  void moveDisplay(bool is_up_button);
  void updateLabel();

  int display_id;
  std::string display_size;
  int index;
  std::unique_ptr<CheckBox> control_checkbox, home_checkbox, away_checkbox;
  std::unique_ptr<Label> display_label;
  std::unique_ptr<Panel> button_panel;
  std::unique_ptr<Button> down_button, up_button;
  DisplaySettingsPage *parent;
};

}  // namespace cszb_scoreboard
