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
    auto* display_panel = new DisplaySettingsPanel(new swx::Panel(wx), i);
    display_settings_panels.push_back(display_panel);
  }

  separator_line = new wxStaticLine(wx);
  window_mode_panel = new wxPanel(wx);

  enable_window_mode =
      new wxCheckBox(window_mode_panel, wxID_ANY, "Enable Window Mode");
  enable_window_mode->SetValue(DisplayConfig::getInstance()->windowedMode());

  number_of_windows_label =
      new wxStaticText(window_mode_panel, wxID_ANY, "# of Windows");
  number_of_windows =
      new wxTextCtrl(window_mode_panel, wxID_ANY,
                     StringUtil::intToString(
                         DisplayConfig::getInstance()->numberOfDisplays()));

  window_size_label =
      new wxStaticText(window_mode_panel, wxID_ANY, "Window Size");
  window_width = new wxTextCtrl(
      window_mode_panel, wxID_ANY,
      StringUtil::intToString(DisplayConfig::getInstance()->windowWidth()));
  window_size_separator_label =
      new wxStaticText(window_mode_panel, wxID_ANY, "x");
  window_height = new wxTextCtrl(
      window_mode_panel, wxID_ANY,
      StringUtil::intToString(DisplayConfig::getInstance()->windowHeight()));

  std::unique_ptr<wxCommandEvent> event(new wxCommandEvent());
  windowModeChanged(*event);
}

void DisplaySettingsPage::positionWidgets() {
  auto* window_mode_sizer = new wxGridBagSizer();
  UiUtil::addToGridBag(window_mode_sizer, enable_window_mode, 0, 0, 1, 2);

  UiUtil::addToGridBag(window_mode_sizer, number_of_windows_label, 1, 0);
  UiUtil::addToGridBag(window_mode_sizer, number_of_windows, 1, 1);

  UiUtil::addToGridBag(window_mode_sizer, window_size_label, 2, 0);
  UiUtil::addToGridBag(window_mode_sizer, window_width, 2, 1);
  UiUtil::addToGridBag(window_mode_sizer, window_size_separator_label, 2, 2);
  UiUtil::addToGridBag(window_mode_sizer, window_height, 2, 3);

  window_mode_panel->SetSizerAndFit(window_mode_sizer);

  int row = 0;
  for (auto* panel : display_settings_panels) {
    UiUtil::addToGridBag(sizer(), panel, row++, 0);
  }

  UiUtil::addToGridBag(sizer(), separator_line, row++, 0, 1, 1,
                       DEFAULT_BORDER_SIZE, wxALL | wxGROW);
  UiUtil::addToGridBag(sizer(), window_mode_panel, row++, 0);

  runSizer();
}

void DisplaySettingsPage::bindEvents() {
  enable_window_mode->Bind(wxEVT_CHECKBOX,
                           &DisplaySettingsPage::windowModeChanged, this);
}

/* Returns true if the display settings are allowable, presents a warning dialog
 * if not (and returns false). */
auto DisplaySettingsPage::validateSettings() -> bool {
  bool has_control = false;
  for (auto* display_panel : display_settings_panels) {
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

  if (enable_window_mode->GetValue()) {
    if (StringUtil::stringToInt(number_of_windows->GetValue()) < 1) {
      wxMessageBox(
          "ERROR: If enabling windows mode, at least one window must be "
          "created.");
      return false;
    }

    if (StringUtil::stringToInt(window_width->GetValue()) < 1 ||
        StringUtil::stringToInt(window_height->GetValue()) < 1) {
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
       enable_window_mode->GetValue()) ||
      (DisplayConfig::getInstance()->windowedModeNumberOfWindows() !=
       StringUtil::stringToInt(number_of_windows->GetValue())) ||
      (DisplayConfig::getInstance()->windowWidth() !=
       StringUtil::stringToInt(window_width->GetValue())) ||
      (DisplayConfig::getInstance()->windowHeight() !=
       StringUtil::stringToInt(window_height->GetValue()))) {
    wxMessageBox(
        "WARNING: Changes to windowed mode will require an application restart "
        "to take effect.");
  }

  DisplayConfig::getInstance()->setWindowedMode(enable_window_mode->GetValue());
  DisplayConfig::getInstance()->setWindowedModeNumberOfWindows(
      StringUtil::stringToInt(number_of_windows->GetValue()));
  DisplayConfig::getInstance()->setWindowWidth(
      StringUtil::stringToInt(window_width->GetValue()));
  DisplayConfig::getInstance()->setWindowHeight(
      StringUtil::stringToInt(window_height->GetValue()));

  DisplayConfig::getInstance()->saveSettings();
}

void DisplaySettingsPage::windowModeChanged(wxCommandEvent& event) {
  if (enable_window_mode->GetValue()) {
    number_of_windows->Enable();
    window_width->Enable();
    window_height->Enable();
  } else {
    number_of_windows->Disable();
    window_width->Disable();
    window_height->Disable();
  }
}

void DisplaySettingsPage::swapDisplays(int a, int b) {
  DisplaySettingsPanel temp(wx, 0);
  temp.copyFrom(display_settings_panels[a]);
  display_settings_panels[a]->copyFrom(display_settings_panels[b]);
  display_settings_panels[b]->copyFrom(&temp);
}

}  // namespace cszb_scoreboard
