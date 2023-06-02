/*
ui/dialog/settings/DisplaySettingsPage.cpp: The page of the settings notebook
which handles display settings.

Copyright 2019-2023 Tracy Beck

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

#include "ui/dialog/settings/DisplaySettingsPage.h"

#include <algorithm>  // for max
#include <string>     // for to_string

#include "ScoreboardCommon.h"       // for DEFAULT_BORDER_SIZE
#include "config.pb.h"              // for ScreenSide
#include "config/DisplayConfig.h"   // for DisplayConfig
#include "config/swx/defs.h"        // for operator|, wxDirection, wxStretch
#include "config/swx/event.h"       // for wxEVT_BUTTON, wxEVT_CHECKBOX
#include "ui/frame/FrameManager.h"  // for FrameManager
#include "ui/frame/MainView.h"      // for MainView
#include "ui/widget/Panel.h"        // for Panel
#include "ui/widget/PopUp.h"        // for PopUp
#include "util/StringUtil.h"        // for StringUtil

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

DisplaySettingsPage::DisplaySettingsPage(swx::Panel *wx, Singleton *singleton)
    : SettingsPage(wx) {
  this->singleton = singleton;
  createControls();
  positionWidgets();
  bindEvents();
}

void DisplaySettingsPage::createControls() {
  display_settings = panel();

  populateDisplays();

  separator_1 = divider();
  separator_2 = divider();
  window_mode_panel = panel();

  enable_window_mode = window_mode_panel->checkBox("Enable Window Mode");
  enable_window_mode->setChecked(singleton->displayConfig()->windowedMode());

  number_of_windows_label = window_mode_panel->label("# of Windows");
  number_of_windows = window_mode_panel->text(
      std::to_string(singleton->displayConfig()->numberOfDisplays()));

  window_size_label = window_mode_panel->label("Window Size");
  window_width = window_mode_panel->text(
      std::to_string(singleton->displayConfig()->windowWidth()));
  window_size_separator_label = window_mode_panel->label("x");
  window_height = window_mode_panel->text(
      std::to_string(singleton->displayConfig()->windowHeight()));

  reset_displays = button("Redetect External Displays", /*exact_fit=*/true);

  windowModeChanged();
}

void DisplaySettingsPage::populateDisplays() {
  for (const auto &panel : display_settings_panels) {
    panel->destroy();
  }
  display_settings_panels.clear();

  int row = 0;
  for (int i = 0; i < singleton->displayConfig()->numberOfDisplays(); ++i) {
    display_settings_panels.push_back(std::make_unique<DisplaySettingsPanel>(
        display_settings->childPanel(), i, this));
    display_settings->addWidget(*display_settings_panels.back(), row++, 0);
  }
}

void DisplaySettingsPage::positionWidgets() {
  window_mode_panel->addWidgetWithSpan(*enable_window_mode, 0, 0, 1, 2);

  window_mode_panel->addWidget(*number_of_windows_label, 1, 0);
  window_mode_panel->addWidget(*number_of_windows, 1, 1);

  window_mode_panel->addWidget(*window_size_label, 2, 0);
  window_mode_panel->addWidget(*window_width, 2, 1);
  window_mode_panel->addWidget(*window_size_separator_label, 2, 2);
  window_mode_panel->addWidget(*window_height, 2, 3);

  window_mode_panel->runSizer();

  int row = 0;
  addWidget(*display_settings, row++, 0);
  addWidget(*separator_1, row++, 0, DEFAULT_BORDER_SIZE, wxALL | wxGROW);
  addWidget(*window_mode_panel, row++, 0);

  addWidget(*separator_2, row++, 0, DEFAULT_BORDER_SIZE, wxALL | wxGROW);
  addWidget(*reset_displays, row++, 0);

  runSizer();
}

void DisplaySettingsPage::bindEvents() {
  enable_window_mode->bind(
      wxEVT_CHECKBOX,
      [this](wxCommandEvent &event) -> void { this->windowModeChanged(); });
  reset_displays->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->resetDisplaysPressed();
  });
}

/* Returns true if the display settings are allowable, presents a warning dialog
 * if not (and returns false). */
auto DisplaySettingsPage::validateSettings() -> bool {
  bool has_control = false;
  for (const auto &display_panel : display_settings_panels) {
    proto::ScreenSide side = display_panel->getSide();
    if (side.control()) {
      has_control = true;
      if (!singleton->displayConfig()->windowedMode() &&
          (side.home() || side.away() || side.extra())) {
        PopUp::Message(
            "ERROR: The Booth Monitor display may not also be a team display.");
        return false;
      }
    }
  }
  if (!has_control) {
    PopUp::Message("ERROR: One window must be selected as a Booth Monitor.");
    return false;
  }

  if (enable_window_mode->checked()) {
    if (StringUtil::stringToInt(number_of_windows->value()) < 1) {
      PopUp::Message(
          "ERROR: If enabling windows mode, at least one window must be "
          "created.");
      return false;
    }

    if (StringUtil::stringToInt(window_width->value()) < 1 ||
        StringUtil::stringToInt(window_height->value()) < 1) {
      PopUp::Message(
          "ERROR: If enabling windows mode, window resolution must be larger "
          "than 0x0.");
      return false;
    }
  }

  return true;
}

void DisplaySettingsPage::saveSettings() {
  // This variable tracks if something has changed that will require the display
  // presenters to be reset.
  bool reset_displays = false;

  for (int i = 0; i < display_settings_panels.size(); ++i) {
    if (singleton->displayConfig()->setDisplayId(
            i, display_settings_panels[i]->getDisplayId())) {
      // If the display order was changed, reset the displays.
      reset_displays = true;
    }
    if (singleton->displayConfig()->setSide(
            i, display_settings_panels[i]->getSide())) {
      // If the display side was changed, reset the displays.
      reset_displays = true;
    }
  }

  // If any of the windowed settings have chagned, re-detect display settings.
  if ((singleton->displayConfig()->windowedMode() !=
       enable_window_mode->checked()) ||
      (singleton->displayConfig()->windowedModeNumberOfWindows() !=
       StringUtil::stringToInt(number_of_windows->value())) ||
      (singleton->displayConfig()->windowWidth() !=
       StringUtil::stringToInt(window_width->value())) ||
      (singleton->displayConfig()->windowHeight() !=
       StringUtil::stringToInt(window_height->value()))) {
    reset_displays = true;
  }

  singleton->displayConfig()->setWindowedMode(enable_window_mode->checked());
  singleton->displayConfig()->setWindowedModeNumberOfWindows(
      StringUtil::stringToInt(number_of_windows->value()));
  singleton->displayConfig()->setWindowWidth(
      StringUtil::stringToInt(window_width->value()));
  singleton->displayConfig()->setWindowHeight(
      StringUtil::stringToInt(window_height->value()));

  singleton->displayConfig()->saveSettings();

  // Something in the displays has changed -- reset them from the saved settings
  // above.
  if (reset_displays) {
    singleton->frameManager()->mainView()->resetDisplays();
  }
}

void DisplaySettingsPage::windowModeChanged() {
  if (enable_window_mode->checked()) {
    number_of_windows->enable();
    window_width->enable();
    window_height->enable();
  } else {
    number_of_windows->disable();
    window_width->disable();
    window_height->disable();
  }
}

void DisplaySettingsPage::resetDisplaysPressed() {
  if (!PopUp::Confirmation(
          "Warning",
          "This will reset all of your display settings -- are you sure?")) {
    return;
  }

  singleton->displayConfig()->detectDisplays(/*force_reload=*/true);
  singleton->displayConfig()->saveSettings();
  // Reset the ui from new newly  saved settings.
  singleton->frameManager()->mainView()->resetDisplays();

  populateDisplays();
  runSizer();
}

void DisplaySettingsPage::swapDisplays(int a, int b) {
  // Create a self-destructing panel to hold our temp, to clean up after we're
  // done.
  std::unique_ptr<Panel> temp_holder = panel(true);
  auto temp = std::make_unique<DisplaySettingsPanel>(temp_holder->childPanel(),
                                                     0, this);
  temp->copyFrom(*display_settings_panels[a]);
  display_settings_panels[a]->copyFrom(*display_settings_panels[b]);
  display_settings_panels[b]->copyFrom(*temp);
}

}  // namespace cszb_scoreboard
