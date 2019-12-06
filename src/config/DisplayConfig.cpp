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
#include "ui/FrameList.h"

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

// Determines which display currently houses the main control window.
int DisplayConfig::primaryDisplay() {
  wxPoint main_size = FrameList::getInstance()->getMainView()->GetPosition();
  for (int i = 0; i < displays.size(); ++i) {
    if (displays[i].getDimensions().Contains(main_size)) {
      return i;
    }
  }
}

// Makes a best guess at which side should have which screen.  This should be
// configurable eventually.
int DisplayConfig::displayForSide(ScreenSide side) {
  int primaryDisplay = this->primaryDisplay();
  switch (numberOfDisplays()) {
    case 0:  // I dunno what went wrong here, but do our best
    case 1:
      // We only have one display, so it's the best we'll ever be able to do.
      // This should only really come up in debug mode, since we shouldn't be
      // trying to present there otherwise.
      return 0;
    case 2:
      // There's only one valid place to display anything, and it's our
      // non-primary monitor.
      if (primaryDisplay == 0) {
        return 1;
      }
      return 0;
    case 3:
    default:  // Currently 2 is our maximum supported displays.
      switch (side) {
        case SIDE_SINGLE:  // Odd, but pick the lowest non-primary
        case SIDE_LEFT:    // Lowest that isn't primary
          if (primaryDisplay == 0) {
            return 1;
          }
          return 0;
        case SIDE_RIGHT:  // Highest between 2 & 1 that isn't primary
          if (primaryDisplay == 2) {
            return 1;
          }
          return 2;
        case SIDE_NONE:  // This is an error, just send it to display 0
        default:
          return 0;
      }
  }
}

}  // namespace cszb_scoreboard
