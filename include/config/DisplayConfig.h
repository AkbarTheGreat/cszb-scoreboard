/*
config/DisplayConfig.h: This class is a configuration singleton which
represents the current known state of the displays attached to this computer.

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

#include <wx/display.h>
#include <wx/wx.h>

#include "config.pb.h"

namespace cszb_scoreboard {

class DisplayConfig {
 public:
  static auto getInstance() -> DisplayConfig*;
  static auto isPrimaryDisplay(proto::DisplayInfo* display_info) -> bool;
  void detectDisplays();
  auto displayDetails(int index) -> proto::DisplayInfo;
  auto numberOfDisplays() -> int;
  void saveSettings();
  // Set the display id to the given monitor index.  Returns true if this
  // was an effective change, false if the values were identical already.
  auto setDisplayId(int index, int id) -> bool;
  void setSide(int index, proto::ScreenSide side);

  auto windowedMode() -> bool;
  auto windowedModeNumberOfWindows() -> int;
  auto windowWidth() -> int;
  auto windowHeight() -> int;

  void setWindowedMode(bool mode);
  void setWindowedModeNumberOfWindows(int num);
  void setWindowWidth(int width);
  void setWindowHeight(int height);

 private:
  proto::DisplayConfig display_config;
  DisplayConfig();
  void detectExternalMonitors();
  void setupWindowedMode();
};
}  // namespace cszb_scoreboard
