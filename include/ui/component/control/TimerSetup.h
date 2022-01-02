/*
ui/component/control/TimerSetup.h: This class is responsible for managing timers
which may be displayed at the bottom of every other view.

Copyright 2021-2022 Tracy Beck

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

#include "ui/component/ScreenText.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/widget/Label.h"
#include "ui/widget/Panel.h"
#include "ui/widget/Text.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class TimerSetup : public ScreenTextController {
 public:
  TimerSetup(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenTextController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<TimerSetup>;

 private:
  void createControls(Panel *control_panel) override;
  void updateScreenText(ScreenText *screen_text) override;
  std::unique_ptr<Label> minutes_label;
  std::unique_ptr<Label> seconds_label;
  std::unique_ptr<Text> minutes_text;
  std::unique_ptr<Text> seconds_text;
};

}  // namespace cszb_scoreboard
