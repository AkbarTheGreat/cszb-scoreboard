/*
ui/component/ControlPanel.h: Holds the controls for changing the contents on a
screen.

Copyright 2019-2025 Tracy Beck

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

#include "ScoreboardCommon.h"                           // for PUBLIC_TEST_ONLY
#include "ui/component/control/ScreenTextController.h"  // for ScreenTextCon...
#include "ui/widget/Notebook.h"                         // for Notebook
#include "util/Singleton.h"                             // for Singleton

class wxAuiNotebookEvent;

namespace cszb_scoreboard {
class ScreenText;

namespace swx {
class Notebook;
}  // namespace swx

class ControlPanel : public Notebook {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit ControlPanel(swx::Notebook* wx)
      : ControlPanel(wx, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void updateScreenTextFromSelected(ScreenText* screen_text);
  auto isSelected(ScreenTextController* controller) -> bool;

  PUBLIC_TEST_ONLY
  ControlPanel(swx::Notebook* wx, Singleton* singleton);

#ifdef SCOREBOARD_TESTING
  auto textController(int index) -> ScreenTextController* {
    return controllers[index].get();
  }
#endif

 private:
  void addController(std::unique_ptr<ScreenTextController> tab,
                     const std::string& name);
  void bindEvents();
  void tabChanged(const wxAuiNotebookEvent& event);
  // Holds a view to these controllers, does not own them.
  std::vector<std::unique_ptr<ScreenTextController>> controllers;
};

}  // namespace cszb_scoreboard
