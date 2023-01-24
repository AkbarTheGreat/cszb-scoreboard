/*
config/DisplayConfig.cpp: This class is a configuration singleton which
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

#include "config/DisplayConfig.h"

#include <google/protobuf/util/message_differencer.h>  // for MessageDiffere...

#include <cassert>  // for assert
#include <cstdint>  // for uint32_t

#include "config/Persistence.h"     // for Persistence
#include "config/Position.h"        // for Position
#include "ui/frame/FrameManager.h"  // for FrameManager
#include "ui/frame/MainView.h"      // for MainView
#include "ui/widget/Display.h"      // for Display
#include "ui/widget/Frame.h"        // for Frame
#include "util/Log.h"               // for LogDebug

namespace cszb_scoreboard {

static const int DEFAULT_WIDTH = 1024;
static const int DEFAULT_HEIGHT = 768;

DisplayConfig::DisplayConfig(SingletonClass c, Singleton *singleton) {
  this->singleton = singleton;
  detectDisplays();
}

void DisplayConfig::detectDisplays() {
  display_config = singleton->persistence()->loadDisplays();

  // If we're in windowed mode, default to a reasonably sized window.
  if (display_config.window_size().width() == 0) {
    display_config.mutable_window_size()->set_x(0);
    display_config.mutable_window_size()->set_y(0);
    display_config.mutable_window_size()->set_width(DEFAULT_WIDTH);
    display_config.mutable_window_size()->set_height(DEFAULT_HEIGHT);
  }

  if (display_config.enable_windowed_mode()) {
    setupWindowedMode();
  } else {
    detectExternalMonitors();
  }

  saveSettings();
}

void DisplayConfig::detectExternalMonitors() {
  uint32_t numscreens = singleton->frameManager()->monitorCount();

#ifdef SCOREBOARD_UI_TEST
  // Force this to be 1 screen for tests, as all of our testing assumed that to
  // be the case anyway.
  numscreens = 1;
#endif

  // Currently, don't re-detect displays if it matches our saved state,
  // otherwise, we'll re-initialize them.  In the future, we may get fancier
  // with trying to save the state of existing displays if new ones are added,
  // or something similar.
  if (numscreens == display_config.displays_size()) {
    LogDebug("Screen count did not change from %d, using saved config",
             numscreens);
    return;
  }
  LogDebug("Screen count changed from %d to %d, reconfiguring",
           display_config.displays_size(), numscreens);

  display_config.clear_displays();
  bool set_home = true;

  for (uint32_t i = 0; i < numscreens; i++) {
    proto::DisplayInfo *display_info = display_config.add_displays();
    display_info->set_id(i);
    Display display = singleton->frameManager()->monitor(i);
    LogDebug("Display %d detected: %s", i, display.debugString().c_str());
    display.geometry(display_info->mutable_dimensions());
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
}

void DisplayConfig::setupWindowedMode() {
  int numscreens = display_config.window_count();

  // Re-initialize the windows if the number of requested windows has changed.
  // This has similar caveats to the external monitor version, but is less
  // severe of a problem, IMHO.
  if (numscreens == display_config.displays_size()) {
    LogDebug("Screen count did not change from %d, using saved config",
             numscreens);
    return;
  }
  LogDebug("Screen count changed from %d to %d, reconfiguring",
           display_config.displays_size(), numscreens);

  display_config.clear_displays();

  for (int i = 0; i < numscreens; i++) {
    proto::DisplayInfo *display_info = display_config.add_displays();
    display_info->set_id(i);

    // Set our size to the configured size.
    display_info->mutable_dimensions()->CopyFrom(display_config.window_size());

    // The lowest monitor will be set to control + home, all others set to away
    // as defaults.
    if (i == 0) {
      display_info->mutable_side()->set_control(true);
      display_info->mutable_side()->set_home(true);
    } else {
      display_info->mutable_side()->set_away(true);
    }
  }
}

auto DisplayConfig::setSide(int index, const proto::ScreenSide &side) -> bool {
  assert(index < display_config.displays_size() && index >= 0);
  // Allocated with new, since display_config will take ownership of it when
  // set.
  google::protobuf::util::MessageDifferencer diff;
  if (diff.Compare(display_config.displays(index).side(), side)) {
    return false;
  }
  auto *side_copy = new proto::ScreenSide(side);
  display_config.mutable_displays(index)->clear_side();
  display_config.mutable_displays(index)->set_allocated_side(side_copy);
  return true;
}

auto DisplayConfig::setDisplayId(int index, int id) -> bool {
  assert(index < display_config.displays_size() && index >= 0);
  if (display_config.mutable_displays(index)->id() == id) {
    return false;
  }
  display_config.mutable_displays(index)->set_id(id);
  return true;
}

void DisplayConfig::saveSettings() {
  LogDebug("Display Config Saving: %s", display_config.DebugString());
  singleton->persistence()->saveDisplays(display_config);
}

auto DisplayConfig::numberOfDisplays() -> int {
  return display_config.displays_size();
}

auto DisplayConfig::displayDetails(int index) -> proto::DisplayInfo {
  assert(index < display_config.displays_size() && index >= 0);
  return display_config.displays(index);
}

// Determines which display currently houses the main control window.
auto DisplayConfig::isPrimaryDisplay(proto::DisplayInfo *display_info) -> bool {
  Frame *main_view = singleton->frameManager()->mainView();
  if (main_view == nullptr) {
    return true;  // Guess that screen 0 is our primary, as we haven't created
                  // our main window yet.
  }
  return main_view->position().isContainedIn(display_info->dimensions());
}

auto DisplayConfig::windowedMode() -> bool {
  return display_config.enable_windowed_mode();
}

void DisplayConfig::setWindowedMode(bool mode) {
  display_config.set_enable_windowed_mode(mode);
}

auto DisplayConfig::windowedModeNumberOfWindows() -> int {
  return display_config.window_count();
}

void DisplayConfig::setWindowedModeNumberOfWindows(int num) {
  display_config.set_window_count(num);
}

auto DisplayConfig::windowWidth() -> int {
  return display_config.window_size().width();
}

void DisplayConfig::setWindowWidth(int width) {
  display_config.mutable_window_size()->set_width(width);
}

auto DisplayConfig::windowHeight() -> int {
  return display_config.window_size().height();
}

void DisplayConfig::setWindowHeight(int height) {
  display_config.mutable_window_size()->set_height(height);
}

}  // namespace cszb_scoreboard
