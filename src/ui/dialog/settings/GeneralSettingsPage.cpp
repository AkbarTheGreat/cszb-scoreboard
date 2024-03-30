/*
ui/dialog/settings/GeneralSettingsPage.cpp: The page of the settings notebook
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

#include "ui/dialog/settings/GeneralSettingsPage.h"

#include <wx/event.h>  // for wxCommandEvent (ptr only), wxEVT_C...

#include "config/GeneralConfig.h"  // for GeneralConfig

namespace cszb_scoreboard {

constexpr double DROP_SHADOW_DISTANCE = 0.08;

GeneralSettingsPage::GeneralSettingsPage(swx::Panel *wx, Singleton *singleton)
    : SettingsPage(wx) {
  this->singleton = singleton;
  createControls();
  positionWidgets();
}

void GeneralSettingsPage::createControls() {
  general_settings = panel();
  enable_drop_shadow = general_settings->checkBox("Enable Drop Shadow");
  enable_drop_shadow->setChecked(
      singleton->generalConfig()->dropShadowDistance() > 0);
}

void GeneralSettingsPage::positionWidgets() {
  general_settings->addWidget(*enable_drop_shadow, 0, 0);
  general_settings->runSizer();

  addWidget(*general_settings, 0, 0);
  runSizer();
}

void GeneralSettingsPage::bindEvents() {
  enable_drop_shadow->bind(
      wxEVT_CHECKBOX,
      [this](wxCommandEvent &event) -> void { this->dropShadowChanged(); });
}

auto GeneralSettingsPage::validateSettings() -> bool {
  // Currently these settings are simple enough they can't really go wrong.
  return true;
}

void GeneralSettingsPage::saveSettings() {
  singleton->generalConfig()->setDropShadowDistance(
      enable_drop_shadow->checked() ? DROP_SHADOW_DISTANCE : -1);
  singleton->generalConfig()->saveSettings();
}

void GeneralSettingsPage::dropShadowChanged() {}

}  // namespace cszb_scoreboard
