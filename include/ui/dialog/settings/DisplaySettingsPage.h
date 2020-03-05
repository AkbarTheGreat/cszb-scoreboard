/*
ui/dialog/settings/DisplaySettingsPage.h: The page of the settings notebook
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
#pragma once

#include <wx/wx.h>

#include "config.pb.h"
#include "config/DisplayConfig.h"
#include "ui/dialog/settings/DisplaySettingsPanel.h"
#include "ui/dialog/settings/SettingsPage.h"

namespace cszb_scoreboard {

class DisplaySettingsPage : public SettingsPage {
 public:
  DisplaySettingsPage(wxWindow *parent);
  void saveSettings() override;
  bool validateSettings() override;

 private:
  std::vector<DisplaySettingsPanel *> display_settings_panels;
};

}  // namespace cszb_scoreboard
