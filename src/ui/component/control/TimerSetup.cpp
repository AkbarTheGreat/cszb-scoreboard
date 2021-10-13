/*
ui/component/control/TimerSetup.cpp: This class is responsible for managing
timers which may be displayed at the bottom of every other view.

Copyright 2021 Tracy Beck

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

#include "ui/component/control/TimerSetup.h"

namespace cszb_scoreboard {

auto TimerSetup ::Create(PreviewPanel *preview_panel, swx::Panel *wx)
    -> std::unique_ptr<TimerSetup> {
  auto control = std::make_unique<TimerSetup>(preview_panel, wx);
  control->initializeWidgets();
  return control;
}

void TimerSetup::createControls(Panel *control_panel) {}

void TimerSetup::updateScreenText(ScreenText *screen_text) {}

}  // namespace cszb_scoreboard
