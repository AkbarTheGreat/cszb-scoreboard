/*
config/DisplayConfig.cpp: This class is a configuration singleton which
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

#include "config/DisplayConfig.h"

#include "config/CommandArgs.h"
#include "config/Persistence.h"
#include "ui/frame/FrameList.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

DisplayConfig *DisplayConfig::singleton_instance = nullptr;

DisplayConfig::DisplayConfig() { detectDisplays(); }

DisplayConfig *DisplayConfig::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new DisplayConfig();
  }
  return singleton_instance;
}

void DisplayConfig::detectDisplays() {
  if (CommandArgs::getInstance()->windowedMode()) {
    setupWindowedMode();
  } else {
    detectExternalMonitors();
  }
}

void DisplayConfig::detectExternalMonitors() {
  int numscreens = wxDisplay::GetCount();

  display_config = Persistence::getInstance()->loadDisplays();

  // Currently, don't re-detect displays if it matches our saved state,
  // otherwise, we'll re-initialize them.  In the future, we may get fancier
  // with trying to save the state of existing displays if new ones are added,
  // or something similar.
  if (numscreens == display_config.displays_size()) {
    wxLogDebug("Screen count did not change from %d, using saved config",
               numscreens);
    return;
  }
  wxLogDebug("Screen count changed from %d to %d, reconfiguring",
             display_config.displays_size(), numscreens);

  display_config.clear_displays();
  bool set_home = true;

  for (int i = 0; i < numscreens; i++) {
    proto::DisplayInfo *display_info = display_config.add_displays();
    display_info->set_id(i);
    wxDisplay display(i);
    ProtoUtil::protoRct(display.GetGeometry(),
                        display_info->mutable_dimensions());
    if (isPrimaryDisplay(display_info)) {
      display_info->mutable_side()->set_control(true);
      if (numscreens == 1) {
        // Create an error "screen" to let the user know we don't expect this to
        // work.
        display_info->mutable_side()->set_error(true);
      }
    } else {
      // The lowest monitor will default to home, all others default to away,
      // aside from the primary.  This can lead to some funky setups for two
      // monitor setups and 4+ monitor setups, but that's easily fixed in
      // settings after startup.
      if (set_home) {
        display_info->mutable_side()->set_home(true);
        set_home = false;
      } else {
        display_info->mutable_side()->set_away(true);
      }
    }
  }

  saveSettings();
}

void DisplayConfig::setupWindowedMode() {
  int numscreens = CommandArgs::getInstance()->numWindows();

  display_config = Persistence::getInstance()->loadDisplays();

  // Re-initialize the windows if the number of requested windows has changed.
  // This has similar caveats to the external monitor version, but is less
  // severe of a problem, IMHO.
  if (numscreens == display_config.displays_size()) {
    wxLogDebug("Screen count did not change from %d, using saved config",
               numscreens);
    return;
  }
  wxLogDebug("Screen count changed from %d to %d, reconfiguring",
             display_config.displays_size(), numscreens);

  display_config.clear_displays();

  for (int i = 0; i < numscreens; i++) {
    proto::DisplayInfo *display_info = display_config.add_displays();
    display_info->set_id(i);
    // If we're in windowed mode, just do a reasonably sized window.
    ProtoUtil::protoRct(wxRect(0, 0, 1024, 768),
                        display_info->mutable_dimensions());

    // The lowest monitor will be set to control + home, all others set to away
    // as defaults.
    if (i == 0) {
      display_info->mutable_side()->set_control(true);
      display_info->mutable_side()->set_home(true);
    } else {
      display_info->mutable_side()->set_away(true);
    }
  }
  saveSettings();
}

void DisplayConfig::setSide(int index, proto::ScreenSide side) {
  proto::ScreenSide *side_copy = new proto::ScreenSide(side);
  display_config.mutable_displays(index)->clear_side();
  display_config.mutable_displays(index)->set_allocated_side(side_copy);
}

bool DisplayConfig::setDisplayId(int index, int id) {
  if (display_config.mutable_displays(index)->id() == id) {
    return false;
  }
  display_config.mutable_displays(index)->set_id(id);
  return true;
}

void DisplayConfig::saveSettings() {
  Persistence::getInstance()->saveDisplays(display_config);
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
