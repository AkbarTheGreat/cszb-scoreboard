/*
config/DisplayConfig.h: This class is a configuration singleton which
represents the current known state of the displays attached to this computer.

Copyright 2019-2020 Tracy Beck

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
  static DisplayConfig *getInstance();
  void detectDisplays();
  proto::DisplayInfo displayDetails(int index);
  int numberOfDisplays();
  bool isPrimaryDisplay(proto::DisplayInfo *display_info);
  void saveSettings();
  void setDisplayId(int index, int id);
  void setSide(int index, proto::ScreenSide side);

 private:
  static DisplayConfig *singleton_instance;
  proto::DisplayConfig display_config;
  DisplayConfig();
};
}  // namespace cszb_scoreboard
