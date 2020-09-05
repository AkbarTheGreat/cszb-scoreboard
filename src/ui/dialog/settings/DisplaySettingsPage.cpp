/*
ui/dialog/settings/DisplaySettingsPage.cpp: The page of the settings notebook
which handles display settings.

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

#include "ui/dialog/settings/DisplaySettingsPage.h"

#include "config/DisplayConfig.h"
#include "ui/UiUtil.h"
#include "util/StringUtil.h"
#include "wx/gbsizer.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

DisplaySettingsPage::DisplaySettingsPage(wxWindow* parent)
    : SettingsPage(parent) {
  createControls();
  positionWidgets();
}

void DisplaySettingsPage::createControls() {
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    DisplaySettingsPanel* display_panel = new DisplaySettingsPanel(this, i);
    display_settings_panels.push_back(display_panel);
  }

  separator_line = new wxStaticLine(this);
  window_mode_panel = new wxPanel(this);

#ifdef ENABLE_WINDOW_MODE_OPTION
  line1_buffer1 = new wxStaticText(window_mode_panel, wxID_ANY, "");
  line1_buffer2 = new wxStaticText(window_mode_panel, wxID_ANY, "");
  line1_buffer3 = new wxStaticText(window_mode_panel, wxID_ANY, "");
  line2_buffer1 = new wxStaticText(window_mode_panel, wxID_ANY, "");
  line2_buffer2 = new wxStaticText(window_mode_panel, wxID_ANY, "");

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
#endif
}

void DisplaySettingsPage::positionWidgets() {
#ifdef ENABLE_WINDOW_MODE_OPTION
  wxGridBagSizer* window_mode_sizer = new wxGridBagSizer();
  UiUtil::addToGridBag(window_mode_sizer, enable_window_mode, 0, 0, 1, 2);

  UiUtil::addToGridBag(window_mode_sizer, number_of_windows_label, 1, 0);
  UiUtil::addToGridBag(window_mode_sizer, number_of_windows, 1, 1);

  UiUtil::addToGridBag(window_mode_sizer, window_size_label, 2, 0);
  UiUtil::addToGridBag(window_mode_sizer, window_width, 2, 1);
  UiUtil::addToGridBag(window_mode_sizer, window_size_separator_label, 2, 2);
  UiUtil::addToGridBag(window_mode_sizer, window_height, 2, 3);

  window_mode_panel->SetSizerAndFit(window_mode_sizer);
#endif

  wxSizer* sizer = UiUtil::sizer(0, 1);

  for (auto panel : display_settings_panels) {
    sizer->Add(panel, 0, wxALL, BORDER_SIZE);
  }

#ifdef ENABLE_WINDOW_MODE_OPTION
  sizer->Add(separator_line, 0, wxALL | wxGROW, BORDER_SIZE);
  sizer->Add(window_mode_panel, 0, wxALL, BORDER_SIZE);
#endif

  SetSizerAndFit(sizer);
}

/* Returns true if the display settings are allowable, presents a warning dialog
 * if not (and returns false). */
bool DisplaySettingsPage::validateSettings() {
  bool has_control = false;
  for (auto display_panel : display_settings_panels) {
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
  DisplayConfig::getInstance()->saveSettings();
}

void DisplaySettingsPage::swapDisplays(int a, int b) {
  DisplaySettingsPanel temp(this, 0);
  temp.copyFrom(display_settings_panels[a]);
  display_settings_panels[a]->copyFrom(display_settings_panels[b]);
  display_settings_panels[b]->copyFrom(&temp);
}

}  // namespace cszb_scoreboard
