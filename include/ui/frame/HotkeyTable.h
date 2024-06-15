/*
ui/frame/HotkeyTable.h: This class manages hotkeys for the application.

Copyright 2019-2024 Tracy Beck

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

// wx/accel needs to be used by clients of this interface as they use macros to
// reference key combinations (like wxACCEL_CTRL).  But no one should use it
// outside of interfacing with this class.
#include <wx/accel.h>  // IWYU pragma: export for wxAcceleratorEntry
#include <wx/defs.h>   // for wxWindowID

#include <vector>  // for vector

#include "util/Singleton.h"

namespace cszb_scoreboard {
class Frame;

class HotkeyTable {
 public:
  explicit HotkeyTable(SingletonClass c) {}
  void addHotkey(int modifier_flags, int key_code, wxWindowID widget_id);
  void installHotkeys(Frame *main_window);

 private:
  std::vector<wxAcceleratorEntry> accel_entries;
};

}  // namespace cszb_scoreboard