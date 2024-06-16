/*
config/GeneralConfig.h: This class is a configuration singleton which
represents generic settings for the scoreboard.

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

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "config.pb.h"         // for GeneralConfig
#include "util/Singleton.h"    // for Singleton, SingletonClass

namespace cszb_scoreboard {

class GeneralConfig {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  explicit GeneralConfig(SingletonClass c)
      : GeneralConfig(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void setDropShadowDistance(double distance);
  auto dropShadowDistance() -> double;
  void saveSettings();

  PUBLIC_TEST_ONLY
  GeneralConfig(SingletonClass c, Singleton *singleton);

 private:
  auto checkGeneralConfig() -> bool;
  proto::GeneralConfig general_config;
  Singleton *singleton;
};
}  // namespace cszb_scoreboard
