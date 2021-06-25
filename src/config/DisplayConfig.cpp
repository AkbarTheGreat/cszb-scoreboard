/*
config/DisplayConfig.cpp: This class is a configuration singleton which
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

#include "config/DisplayConfig.h"

#include "config/Persistence.h"
#include "ui/frame/FrameList.h"
#include "ui/widget/Frame.h"
#include "util/Log.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

static const int DEFAULT_WIDTH = 1024;
static const int DEFAULT_HEIGHT = 768;

DisplayConfig::DisplayConfig() { detectDisplays(); }

auto DisplayConfig::getInstance() -> DisplayConfig * {
  static DisplayConfig singleton;
  return &singleton;
}

void DisplayConfig::detectDisplays() {
  display_config = Persistence::getInstance()->loadDisplays();

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
  int numscreens = wxDisplay::GetCount();

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

void DisplayConfig::setSide(int index, proto::ScreenSide side) {
  // Allocated with new, since display_config will take ownership of it when
  // set.
  auto *side_copy = new proto::ScreenSide(std::move(side));
  display_config.mutable_displays(index)->clear_side();
  display_config.mutable_displays(index)->set_allocated_side(side_copy);
}

auto DisplayConfig::setDisplayId(int index, int id) -> bool {
  if (display_config.mutable_displays(index)->id() == id) {
    return false;
  }
  display_config.mutable_displays(index)->set_id(id);
  return true;
}

void DisplayConfig::saveSettings() {
  Persistence::getInstance()->saveDisplays(display_config);
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
  Frame *main_view = FrameList::getInstance()->getMainView();
  if (main_view == nullptr) {
    return true;  // Guess that screen 0 is our primary, as we haven't created
                  // our main window yet.
  }
  wxPoint main_size = main_view->GetPosition();
  return (ProtoUtil::wxRct(display_info->dimensions()).Contains(main_size));
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
