/*
util/TimerManager.h: Singleton which handles the current time left on the game
timer.

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

#include <util/StringUtil.h>

#include <chrono>
#include <memory>

#include "ScoreboardCommon.h"
#include "ui/event/AutoRefreshTimer.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {

class TimerManager {
 public:
  explicit TimerManager(SingletonClass c)
      : TimerManager(c, Singleton::getInstance()) {}
  [[nodiscard]] auto timerOn() const -> bool { return timer_displayed; }
  auto displayTime() -> std::string;

  PUBLIC_TEST_ONLY
  TimerManager(SingletonClass c, Singleton *singleton);

 private:
  auto timeLeft() -> std::chrono::seconds;

  std::unique_ptr<AutoRefreshTimer> refresh_timer;
  bool timer_displayed;
  bool timer_running;
  std::chrono::seconds timer_end;
};

}  // namespace cszb_scoreboard
