/*
ui/dialog/settings/DisplaySettingsPage.cpp: The page of the settings notebook
which handles display settings.

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

#include "ui/dialog/settings/DisplaySettingsPage.h"

#include "config/DisplayConfig.h"
#include "ui/UiUtil.h"
#include "ui/widget/Panel.h"
#include "util/StringUtil.h"
#include "wx/gbsizer.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

DisplaySettingsPage::DisplaySettingsPage(swx::Panel* wx) : SettingsPage(wx) {
  createControls();
  positionWidgets();
  bindEvents();
}

void DisplaySettingsPage::createControls() {
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    display_settings_panels.push_back(
        std::make_unique<DisplaySettingsPanel>(childPanel(), i, this));
  }

  separator_line = divider();
  window_mode_panel = panel();

  enable_window_mode = window_mode_panel->checkBox("Enable Window Mode");
  enable_window_mode->setChecked(DisplayConfig::getInstance()->windowedMode());

  number_of_windows_label = window_mode_panel->label("# of Windows");
  number_of_windows = window_mode_panel->text(StringUtil::intToString(
      DisplayConfig::getInstance()->numberOfDisplays()));

  window_size_label = window_mode_panel->label("Window Size");
  window_width = window_mode_panel->text(
      StringUtil::intToString(DisplayConfig::getInstance()->windowWidth()));
  window_size_separator_label = window_mode_panel->label("x");
  window_height = window_mode_panel->text(
      StringUtil::intToString(DisplayConfig::getInstance()->windowHeight()));

  windowModeChanged();
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
  for (const auto& panel : display_settings_panels) {
    addWidget(*panel, row++, 0);
  }

  addWidget(*separator_line, row++, 0, DEFAULT_BORDER_SIZE, wxALL | wxGROW);
  addWidget(*window_mode_panel, row++, 0);

  runSizer();
}

void DisplaySettingsPage::bindEvents() {
  enable_window_mode->bind(
      wxEVT_CHECKBOX,
      [this](wxCommandEvent& event) -> void { this->windowModeChanged(); });
}

/* Returns true if the display settings are allowable, presents a warning dialog
 * if not (and returns false). */
auto DisplaySettingsPage::validateSettings() -> bool {
  bool has_control = false;
  for (const auto& display_panel : display_settings_panels) {
    proto::ScreenSide side = display_panel->getSide();
    if (side.control()) {
      has_control = true;
      if (!DisplayConfig::getInstance()->windowedMode() &&
          (side.home() || side.away() || side.extra())) {
        wxMessageBox(
            "ERROR: The Booth Monitor display may not also be a team display.");
        return false;
      }
    }
  }
  if (!has_control) {
    wxMessageBox("ERROR: One window must be selected as a Booth Monitor.");
    return false;
  }

  if (enable_window_mode->checked()) {
    if (StringUtil::stringToInt(number_of_windows->value()) < 1) {
      wxMessageBox(
          "ERROR: If enabling windows mode, at least one window must be "
          "created.");
      return false;
    }

    if (StringUtil::stringToInt(window_width->value()) < 1 ||
        StringUtil::stringToInt(window_height->value()) < 1) {
      wxMessageBox(
          "ERROR: If enabling windows mode, window resolution must be larger "
          "than 0x0.");
      return false;
    }
  }

  return true;
}

void DisplaySettingsPage::saveSettings() {
  bool warnedAboutOrderChange = false;

  for (int i = 0; i < display_settings_panels.size(); ++i) {
    if (DisplayConfig::getInstance()->setDisplayId(
            i, display_settings_panels[i]->getDisplayId()) &&
        !warnedAboutOrderChange) {
      wxMessageBox(
          "WARNING: You have changed monitor ordering.  To see this take "
          "effect, you must restart the application.");
      warnedAboutOrderChange = true;
    }
    DisplayConfig::getInstance()->setSide(
        i, display_settings_panels[i]->getSide());
  }

  // If any of the windowed settings have chagned, re-detect display settings.
  if ((DisplayConfig::getInstance()->windowedMode() !=
       enable_window_mode->checked()) ||
      (DisplayConfig::getInstance()->windowedModeNumberOfWindows() !=
       StringUtil::stringToInt(number_of_windows->value())) ||
      (DisplayConfig::getInstance()->windowWidth() !=
       StringUtil::stringToInt(window_width->value())) ||
      (DisplayConfig::getInstance()->windowHeight() !=
       StringUtil::stringToInt(window_height->value()))) {
    wxMessageBox(
        "WARNING: Changes to windowed mode will require an application restart "
        "to take effect.");
  }

  DisplayConfig::getInstance()->setWindowedMode(enable_window_mode->checked());
  DisplayConfig::getInstance()->setWindowedModeNumberOfWindows(
      StringUtil::stringToInt(number_of_windows->value()));
  DisplayConfig::getInstance()->setWindowWidth(
      StringUtil::stringToInt(window_width->value()));
  DisplayConfig::getInstance()->setWindowHeight(
      StringUtil::stringToInt(window_height->value()));

  DisplayConfig::getInstance()->saveSettings();
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
