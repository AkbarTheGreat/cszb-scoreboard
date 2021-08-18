/*
ui/event/UpdateTimer.h: A timer which periodically checks for updates.

Copyright 2019-2021 Tracy Beck

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

#include <ui/widget/timer.h>

namespace cszb_scoreboard {

constexpr int AUTO_UPDATE_DELAY = 6 * 60 * 60 * 1000;

class Frame;

class UpdateTimer : public Timer {
 public:
  explicit UpdateTimer(Frame *main_view);
  void start() override;

 private:
  Frame *main_view;

  // Retry every six hours to look for an update.
  auto periodMillis() -> int override { return AUTO_UPDATE_DELAY; }
  void execute() override;
};

}  // namespace cszb_scoreboard