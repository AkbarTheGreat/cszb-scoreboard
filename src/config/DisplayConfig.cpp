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

#include "config/Persistence.h"
#include "ui/FrameList.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

DisplayConfig *DisplayConfig::singleton_instance;

DisplayConfig::DisplayConfig() { detectDisplays(); }

DisplayConfig *DisplayConfig::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new DisplayConfig();
  }
  return singleton_instance;
}

void DisplayConfig::detectDisplays() {
  int numscreens = wxDisplay::GetCount();

  display_config = Persistence::loadDisplays();

  // Currently, don't re-detect displays if it matches our saved state,
  // otherwise, we'll re-initialize them.  In the future, we may get fancier
  // with trying to save the state of existing displays if new ones are added,
  // or something similar.
  if (numscreens == display_config.displays_size()) {
    wxLogDebug("Screen count did not change from %d, using saved config",
               numscreens);
#ifndef WXDEBUG
    // Don't actually exit in debug mode, since we may change things around
    // while developing.
    return;
#endif
  }
  wxLogDebug("Screen count changed from %d to %d, reconfiguring",
             display_config.displays_size(), numscreens);
  display_config.clear_displays();
  bool set_home = true;

  for (int i = 0; i < numscreens; i++) {
    wxDisplay display(i);
    proto::DisplayInfo *display_info = display_config.add_displays();
    ProtoUtil::protoRct(display.GetGeometry(),
                        display_info->mutable_dimensions());
    if (isPrimaryDisplay(display_info)) {
      display_info->mutable_side()->set_control(true);
      if (numscreens == 1) {
#ifdef WXDEBUG
        // For debugging, we display home as if it was a second monitor.
        display_info->mutable_side()->set_home(true);
#else
        // Create an error "screen" to let the user know we don't expect this to
        // work.
        display_info->mutable_side()->set_error(true);
#endif
      }
    } else {
      // The lowest monitor will default to home, the highest away, aside from
      // the primary.  If we have 4 or more monitors, we don't yet support that
      // (despite the proto having some ideas about that baked in).  We also
      // don't auto-attempt to do a split-screen view at the moment, but that
      // should be possible if there are only two monitors, auto-config the
      // second monitor to be home + away.
      if (set_home) {
        display_info->mutable_side()->set_home(true);
        set_home = false;
      } else {
        display_info->mutable_side()->set_away(true);
      }
    }
  }
  Persistence::saveDisplays(display_config);
}

int DisplayConfig::numberOfDisplays() { return display_config.displays_size(); }

proto::DisplayInfo DisplayConfig::displayDetails(int index) {
  assert(index < display_config.displays_size() && index >= 0);
  return display_config.displays(index);
}

// Determines which display currently houses the main control window.
bool DisplayConfig::isPrimaryDisplay(proto::DisplayInfo *display_info) {
  wxFrame *main_view = FrameList::getInstance()->getMainView();
  if (main_view == nullptr) {
    return 0;  // Guess that screen 0 is our primary, as we haven't created our
               // main window yet.
  }
  wxPoint main_size = main_view->GetPosition();
  if (ProtoUtil::wxRct(display_info->dimensions()).Contains(main_size)) {
    return true;
  }
  return false;
}

}  // namespace cszb_scoreboard
