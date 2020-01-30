/*
ui/frame/HotkeyTable.cpp: This class manages hotkeys for the application.

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

#include "ui/frame/HotkeyTable.h"

namespace cszb_scoreboard {

HotkeyTable *HotkeyTable::singleton_instance;

HotkeyTable *HotkeyTable::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new HotkeyTable();
  }
  return singleton_instance;
}

HotkeyTable::HotkeyTable() {}

void HotkeyTable::addHotkey(int modifier_flags, int key_code,
                            wxWindowID widget_id) {
  accel_entries.push_back(wxAcceleratorEntry());
  accel_entries.back().Set(modifier_flags, key_code, widget_id);
}

void HotkeyTable::installHotkeys(wxFrame *main_window) {
  main_window->SetAcceleratorTable(
      wxAcceleratorTable(accel_entries.size(), accel_entries.data()));
}

}  // namespace cszb_scoreboard