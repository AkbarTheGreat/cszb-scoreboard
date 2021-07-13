/*
ui/dialog/settings/TeamSettingsPage.h: The page of the settings notebook which
handles team based settings.

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

#include <memory>                                  // for unique_ptr
#include <vector>                                  // for vector

#include "ui/dialog/settings/SettingsPage.h"       // for SettingsPage
#include "ui/dialog/settings/TeamSettingsPanel.h"  // for TeamSettingsPanel

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

class TeamSettingsPage : public SettingsPage {
 public:
  explicit TeamSettingsPage(swx::Panel *wx);
  void saveSettings() override;
  void swapTeams(int a, int b);
  auto validateSettings() -> bool override;

 private:
  std::vector<std::unique_ptr<TeamSettingsPanel>> team_settings_panels;
};

}  // namespace cszb_scoreboard
