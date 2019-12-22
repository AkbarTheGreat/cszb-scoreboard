/*
config/DisplayConfig.h: This class is a configuration singleton which
represents the current known state of the displays attached to this computer.

Copyright 2019 Tracy Beck

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

#include <vector>

#include "proto/config.pb.h"
#include "ui/ScreenSide.h"

namespace cszb_scoreboard {

class DisplayInfo {
 public:
  DisplayInfo(wxRect dimensions);
  inline wxRect getDimensions() { return dimensions; }

 private:
  wxRect dimensions;
};

class DisplayConfig {
 public:
  static DisplayConfig *getInstance();
  void detectDisplays();
  DisplayInfo displayDetails(int index);
  int displayForSide(ScreenSide side);
  int numberOfDisplays();
  int primaryDisplay();

 private:
  static DisplayConfig *singleton_instance;
  std::vector<proto::DisplayInfo> displays;
  DisplayConfig();
};

}  // namespace cszb_scoreboard
