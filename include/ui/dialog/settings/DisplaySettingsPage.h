/*
ui/dialog/settings/DisplaySettingsPage.h: The page of the settings notebook
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
#pragma once

#include <wx/statline.h>
#include <wx/wx.h>

#include "config.pb.h"
#include "config/DisplayConfig.h"
#include "ui/dialog/settings/DisplaySettingsPanel.h"
#include "ui/dialog/settings/SettingsPage.h"

namespace cszb_scoreboard {

class DisplaySettingsPage : public SettingsPage {
 public:
  explicit DisplaySettingsPage(swx::Panel *wx);
  void saveSettings() override;
  auto validateSettings() -> bool override;
  void swapDisplays(int a, int b);

 private:
  void bindEvents();
  void createControls();
  void positionWidgets();
  void windowModeChanged(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
                               // wxWidgets callback.

  std::vector<DisplaySettingsPanel *> display_settings_panels;
  wxPanel *window_mode_panel;
  wxStaticText *number_of_windows_label, *window_size_label,
      *window_size_separator_label;
  wxCheckBox *enable_window_mode;
  wxTextCtrl *number_of_windows, *window_width, *window_height;
  wxStaticLine *separator_line;
};

}  // namespace cszb_scoreboard
