/*
config/DisplayConfig.h: This class is a configuration singleton which
represents the current known state of the displays attached to this computer.

Copyright 2019-2022 Tracy Beck

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

#include "ScoreboardCommon.h"
#include "config.pb.h"  // for DisplayConfig, DisplayInfo, ScreenSide (ptr o...
#include "util/Singleton.h"

namespace cszb_scoreboard {

class DisplayConfig {
 public:
  explicit DisplayConfig(SingletonClass c)
      : DisplayConfig(c, Singleton::getInstance()) {}
  auto isPrimaryDisplay(proto::DisplayInfo *display_info) -> bool;
  void detectDisplays(bool force_reload = false);
  auto displayDetails(int index) -> proto::DisplayInfo;
  auto numberOfDisplays() -> int;
  void saveSettings();
  // Set the display id to the given monitor index.  Returns true if this
  // was an effective change, false if the values were identical already.
  auto setDisplayId(int index, int id) -> bool;
  auto setSide(int index, const proto::ScreenSide &side) -> bool;

  auto windowedMode() -> bool;
  auto windowedModeNumberOfWindows() -> int;
  auto windowWidth() -> int;
  auto windowHeight() -> int;

  void setWindowedMode(bool mode);
  void setWindowedModeNumberOfWindows(int num);
  void setWindowWidth(int width);
  void setWindowHeight(int height);

  PUBLIC_TEST_ONLY
  DisplayConfig(SingletonClass c, Singleton *singleton);
  auto displayConfig() -> proto::DisplayConfig { return display_config; }

 private:
  proto::DisplayConfig display_config;
  Singleton *singleton;
  void detectExternalMonitors(bool force_reload);
  void setupWindowedMode();
};
}  // namespace cszb_scoreboard
