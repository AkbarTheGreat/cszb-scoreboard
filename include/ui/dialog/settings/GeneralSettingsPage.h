/*
ui/dialog/settings/GeneralSettingsPage.h: The page of the settings notebook
which handles general/miscellaneous settings.

Copyright 2024 Tracy Beck

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

#include "ScoreboardCommon.h"                 // for PUBLIC_TEST_ONLY
#include "ui/dialog/settings/SettingsPage.h"  // for SettingsPage
#include "ui/widget/CheckBox.h"               // for CheckBox
#include "ui/widget/Panel.h"                  // for Panel
#include "util/Singleton.h"                   // for Singleton

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

class GeneralSettingsPage : public SettingsPage {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  explicit GeneralSettingsPage(swx::Panel *wx)
      : GeneralSettingsPage(wx, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void saveSettings() override;
  auto validateSettings() -> bool override;

  PUBLIC_TEST_ONLY
  explicit GeneralSettingsPage(swx::Panel *wx, Singleton *singleton);

 private:
  void bindEvents();
  void createControls();
  void positionWidgets();
  void dropShadowChanged();

  std::unique_ptr<Panel> general_settings;
  std::unique_ptr<CheckBox> enable_drop_shadow;
  Singleton *singleton;
};

}  // namespace cszb_scoreboard
