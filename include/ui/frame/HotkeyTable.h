/*
ui/frame/HotkeyTable.h: This class manages hotkeys for the application.

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

#include <vector>

namespace cszb_scoreboard {

class HotkeyTable {
 public:
  static HotkeyTable* getInstance();
  void addHotkey(int modifier_flags, int key_code, wxWindowID widget_id);
  void installHotkeys(wxFrame* main_window);

 private:
  static HotkeyTable* singleton_instance;
  std::vector<wxAcceleratorEntry> accel_entries;
  HotkeyTable();
};

}  // namespace cszb_scoreboard