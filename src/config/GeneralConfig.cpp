/*
config/GeneralConfig.cpp: This class is a configuration singleton which
represents generic settings for the scoreboard.

Copyright 2024-2025 Tracy Beck

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

#include "config/GeneralConfig.h"

#include "config/Persistence.h"  // for Persistence

namespace cszb_scoreboard {

GeneralConfig::GeneralConfig(SingletonClass c, Singleton *singleton) {
  this->singleton = singleton;
  general_config = singleton->persistence()->loadGeneralConfig();
  bool settings_changed = false;

  settings_changed = checkGeneralConfig() || settings_changed;

  if (settings_changed) {
    saveSettings();
  }
}

auto GeneralConfig::checkGeneralConfig() -> bool {
  bool config_updated = false;

  // Establish any defaults, if necessary.
  if (general_config.drop_shadow_depth() == 0) {
    general_config.set_drop_shadow_depth(-1);
    config_updated = true;
  }

  return config_updated;
}

auto GeneralConfig::dropShadowDistance() -> double {
  return general_config.drop_shadow_depth();
}

void GeneralConfig::setDropShadowDistance(double distance) {
  general_config.set_drop_shadow_depth(distance);
}

void GeneralConfig::saveSettings() {
  singleton->persistence()->saveGeneralConfig(general_config);
}

}  // namespace cszb_scoreboard
