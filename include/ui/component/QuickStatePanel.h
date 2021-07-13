/*
ui/component/QuickStatePanel.h: This panel provides quick-settable screens.

Copyright 2020-2021 Tracy Beck

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

#include <vector>                     // for vector
#include <memory>                     // for unique_ptr
#include <string>                     // for string

#include "ui/component/ScreenText.h"  // for ScreenText
#include "ui/widget/Panel.h"          // for Panel

class wxButton;
class wxCommandEvent;
class wxMouseEvent;

namespace cszb_scoreboard {

class QuickStatePanel;

namespace swx {
class Panel;
}  // namespace swx

class QuickStateEntry : public ScreenText {
 public:
  QuickStateEntry(swx::Panel* wx, QuickStatePanel* parent, int id);

 private:
  static auto tooltipText(char command_character) -> std::string;
  void bindEvents(int id);
  void executeShortcut();
  void setShortcut();

  // Thin wrappers just to include the appropriate event objects in the
  // signatures.
  void executeShortcutFromButton(
      wxCommandEvent&
          event) {  // NOLINT(google-runtime-references) wxWidgets callback.
    executeShortcut();
  }
  void executeShortcutFromPanel(
      wxMouseEvent& event) {  // NOLINT(google-runtime-references)
                              // wxWidgets callback.
    executeShortcut();
  }
  void setShortcutFromButton(
      wxCommandEvent& event) {  // NOLINT(google-runtime-references)
                                // wxWidgets callback.
    setShortcut();
  }
  void setShortcutFromPanel(
      wxMouseEvent& event) {  // NOLINT(google-runtime-references)
                              // wxWidgets callback.
    setShortcut();
  }

  QuickStatePanel* parent; // unowned pointer to the parent of this object.
  wxButton *set_button, *execute_button;
  bool initialized = false;
};

class QuickStatePanel : public Panel {
 public:
  explicit QuickStatePanel(swx::Panel* wx);
  void executeShortcut(QuickStateEntry* entry);
  void setShortcut(QuickStateEntry* entry);

 private:
  void positionWidgets();

  std::vector<std::unique_ptr<QuickStateEntry>> entries;
};

}  // namespace cszb_scoreboard
