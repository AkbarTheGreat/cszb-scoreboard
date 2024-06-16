/*
ui/dialog/settings/DisplaySettingsPage.h: The page of the settings notebook
which handles display settings.

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
#include <vector>  // for vector

#include "ScoreboardCommon.h"
#include "ui/dialog/settings/DisplaySettingsPanel.h"  // for DisplaySettings...
#include "ui/dialog/settings/SettingsPage.h"          // for SettingsPage
#include "ui/widget/Button.h"
#include "ui/widget/CheckBox.h"  // for CheckBox
#include "ui/widget/Divider.h"   // for Divider
#include "ui/widget/Label.h"     // for Label
#include "ui/widget/Panel.h"     // for Panel
#include "ui/widget/Text.h"      // for Text
#include "util/Singleton.h"

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

class DisplaySettingsPage : public SettingsPage {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  explicit DisplaySettingsPage(swx::Panel *wx)
      : DisplaySettingsPage(wx, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void saveSettings() override;
  auto validateSettings() -> bool override;
  void swapDisplays(int a, int b);

  PUBLIC_TEST_ONLY
  explicit DisplaySettingsPage(swx::Panel *wx, Singleton *singleton);

 private:
  void bindEvents();
  void createControls();
  void positionWidgets();
  void populateDisplays();
  void windowModeChanged();
  void resetDisplaysPressed();

  std::vector<std::unique_ptr<DisplaySettingsPanel>> display_settings_panels;
  std::unique_ptr<Panel> display_settings, window_mode_panel;
  std::unique_ptr<Label> number_of_windows_label, window_size_label,
      window_size_separator_label;
  std::unique_ptr<Text> number_of_windows, window_width, window_height;
  std::unique_ptr<CheckBox> enable_window_mode;
  std::unique_ptr<Divider> separator_1, separator_2;
  std::unique_ptr<Button> reset_displays;
  Singleton *singleton;
};

}  // namespace cszb_scoreboard
