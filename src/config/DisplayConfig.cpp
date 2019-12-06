/*
config/DisplayConfig.cpp: This class is a configuration singleton which
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
#include "config/DisplayConfig.h"

namespace cszb_scoreboard {

DisplayConfig* DisplayConfig::singleton_instance = new DisplayConfig();

DisplayConfig::DisplayConfig() { detectDisplays(); }

DisplayConfig* DisplayConfig::getInstance() { return singleton_instance; }

void DisplayConfig::detectDisplays() {
  int numscreens = wxDisplay::GetCount();
  for (int i = 0; i < numscreens; i++) {
    wxDisplay display(i);
    DisplayInfo display_info(display.GetGeometry());
    displays.push_back(display_info);
  }
}

int DisplayConfig::numberOfDisplays() { return displays.size(); }

DisplayInfo DisplayConfig::displayDetails(int index) {
  assert(index < displays.size());
  assert(index >= 0);
  return displays[index];
}

DisplayInfo::DisplayInfo(wxRect dimensions) { this->dimensions = dimensions; }
}  // namespace cszb_scoreboard
