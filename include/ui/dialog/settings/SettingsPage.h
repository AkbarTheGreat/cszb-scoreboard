/*
ui/dialog/settings/SettingsPage.h: A page in the settings notebook.  Handles
loading data in from a config object and validating it before saving it.

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

namespace cszb_scoreboard {

class SettingsPage : public wxPanel {
 public:
  virtual void saveSettings() = 0;
  virtual auto validateSettings() -> bool = 0;

 protected:
  explicit SettingsPage(wxWindow *parent) : wxPanel(parent)  {}
  ~SettingsPage() override = default;
};

}  // namespace cszb_scoreboard
