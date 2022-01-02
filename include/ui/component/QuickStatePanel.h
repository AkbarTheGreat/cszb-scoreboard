/*
ui/component/QuickStatePanel.h: This panel provides quick-settable screens.

Copyright 2020-2022 Tracy Beck

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

#include <memory>  // for unique_ptr
#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"
#include "ui/component/ScreenText.h"  // for ScreenText
#include "ui/widget/Button.h"         // for Button
#include "ui/widget/Panel.h"          // for Panel
#include "util/Singleton.h"

namespace cszb_scoreboard {

namespace swx {
class Panel;
}  // namespace swx

class QuickStateEntry : public ScreenText {
 public:
  QuickStateEntry(swx::Panel *wx, int id, Singleton *singleton);

 private:
  static auto tooltipText(char command_character) -> std::string;
  void bindEvents(int id);
  void executeShortcut();
  void setShortcut();

  std::unique_ptr<Button> set_button, execute_button;
  Singleton *singleton;
  bool initialized = false;
};

class QuickStatePanel : public Panel {
 public:
  explicit QuickStatePanel(swx::Panel *wx)
      : QuickStatePanel(wx, Singleton::getInstance()) {}
  static void executeShortcut(QuickStateEntry *entry, Singleton *singleton);
  static void setShortcut(QuickStateEntry *entry, Singleton *singleton);

  PUBLIC_TEST_ONLY
  QuickStatePanel(swx::Panel *wx, Singleton *singleton);

 private:
  void positionWidgets();

  std::vector<std::unique_ptr<QuickStateEntry>> entries;
};

}  // namespace cszb_scoreboard
