/*
ui/component/control/TimerSetup.h: This class is responsible for managing timers
which may be displayed at the bottom of every other view.

Copyright 2021-2025 Tracy Beck

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

#include <cstdint>  // for int64_t
#include <memory>   // for unique_ptr

#include "ui/component/control/ScreenTextController.h"  // for ScreenTextCon...
#include "ui/widget/Button.h"                           // for Button
#include "ui/widget/Label.h"                            // for Label
#include "ui/widget/Text.h"                             // for Text
#include "ui/widget/Toggle.h"                           // for Toggle

namespace cszb_scoreboard {
class Panel;
class ScreenText;
namespace swx {
class Panel;
}  // namespace swx

class TimerSetup : public ScreenTextController {
 public:
  explicit TimerSetup(swx::Panel *wx)
      : ScreenTextController(wx,
                             /*display_update_screens=*/false) {}
  static auto Create(swx::Panel *wx) -> std::unique_ptr<TimerSetup>;

 private:
  void createControls(Panel *control_panel) override;
  void updateScreenText(ScreenText *screen_text) override;
  void bindEvents();
  void positionWidgets(Panel *control_panel);
  void startOrStopTimer();
  void showOrHideTimer();
  void resetTimer();
  auto secondsFromText() -> int64_t;
  void timeUpdated();
  void fixButtons();

  int64_t last_set_timer_seconds = 0;
  std::unique_ptr<Label> minutes_label;
  std::unique_ptr<Label> seconds_label;
  std::unique_ptr<Text> minutes_text;
  std::unique_ptr<Text> seconds_text;
  std::unique_ptr<Toggle> start_stop_button;
  std::unique_ptr<Toggle> show_hide_button;
  std::unique_ptr<Button> reset_button;
};

}  // namespace cszb_scoreboard
