/*
util/TimerManager.h: Singleton which handles the current time left on the game
timer.

Copyright 2021-2024 Tracy Beck

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

#include <chrono>   // for seconds
#include <cstdint>  // for int64_t
#include <memory>   // for unique_ptr
#include <string>   // for string

#include "ScoreboardCommon.h"           // for PUBLIC_TEST_ONLY
#include "ui/event/AutoRefreshTimer.h"  // for AutoRefreshTimer
#include "util/Singleton.h"             // for Singleton, SingletonClass

namespace cszb_scoreboard {

class TimerManager {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit TimerManager(SingletonClass c)
      : TimerManager(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  [[nodiscard]] auto timerOn() const -> bool { return timer_displayed; }
  [[nodiscard]] auto timerRunning() const -> bool { return timer_running; }
  auto displayTime() -> std::string;
  void setTime(int64_t seconds);
  void startTimer();
  void pauseTimer();
  void showTimer();
  void hideTimer();

  PUBLIC_TEST_ONLY
  TimerManager(SingletonClass c, Singleton *singleton);

 private:
  auto timeLeft() -> std::chrono::seconds;

  std::unique_ptr<AutoRefreshTimer> refresh_timer;
  bool timer_displayed;
  bool timer_running;
  // Tracks the time to the end of an active timer.
  std::chrono::seconds timer_end;
  // Tracks the static time left of a paused timer.
  std::chrono::seconds time_left;
};

}  // namespace cszb_scoreboard
