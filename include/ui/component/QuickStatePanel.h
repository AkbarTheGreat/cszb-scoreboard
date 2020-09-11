/*
ui/component/QuickStatePanel.h: This panel provides quick-settable screens.

Copyright 2020 Tracy Beck

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

#include "ui/component/ScreenText.h"

namespace cszb_scoreboard {

class QuickStateEntry {
 public:
  QuickStateEntry(wxPanel* parent, int id);
  ScreenText* screen() { return screen_text; }

 private:
  void bindEvents(int id);
  void executeShortcut();
  void setShortcut();
  std::string tooltipText(char command_character);

  // Thin wrappers just to include the appropriate event objects in the
  // signatures.
  void executeShortcutFromButton(wxCommandEvent& event) { executeShortcut(); }
  void executeShortcutFromPanel(wxMouseEvent& event) { executeShortcut(); }
  void setShortcutFromButton(wxCommandEvent& event) { setShortcut(); }
  void setShortcutFromPanel(wxMouseEvent& event) { setShortcut(); }

  ScreenText* screen_text;
  wxButton *set_button, *execute_button;
  bool initialized = false;
};

class QuickStatePanel : public wxPanel {
 public:
  QuickStatePanel(wxWindow* parent);
  void executeShortcut(ScreenText* screen);
  void setShortcut(ScreenText* screen);

 private:
  ~QuickStatePanel();
  void positionWidgets();

  std::vector<QuickStateEntry*> entries;
};

}  // namespace cszb_scoreboard
