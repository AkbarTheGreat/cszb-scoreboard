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

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

DisplaySettingsPage::DisplaySettingsPage(wxWindow* parent)
    : SettingsPage(parent) {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    DisplaySettingsPanel* display_panel = new DisplaySettingsPanel(this, i);
    display_settings_panels.push_back(display_panel);
    sizer->Add(display_panel, 0, wxALL, BORDER_SIZE);
  }

  SetSizerAndFit(sizer);
}

/* Returns true if the display settings are allowable, presents a warning dialog
 * if not (and returns false). */
bool DisplaySettingsPage::validateSettings() {
  bool has_control = false;
  for (auto display_panel : display_settings_panels) {
    proto::ScreenSide side = display_panel->getSide();
    if (side.control()) {
      // Two controls, that's problematic.
      if (has_control) {
        wxMessageBox(
            "ERROR: Only one window may be selected as a Booth Monitor.");
        return false;
      }
      has_control = true;
      if (side.home() || side.away() || side.extra()) {
// This can't be checked in debug, because we break this rule often in debug
// mode.
#ifndef WXDEBUG
        wxMessageBox(
            "ERROR: The Booth Monitor display may not also be a team display.");
        return false;
#endif
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
