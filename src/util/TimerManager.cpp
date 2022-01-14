/*
util/TimerManager.cpp: Singleton which handles the current time left on the game
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

#include "util/TimerManager.h"

namespace cszb_scoreboard {

TimerManager::TimerManager(SingletonClass c, Singleton *singleton) {
  // TODO(#11): When making methods to start/stop display & the timer, move the
  // creation/destruction of refresh timer into the method that stops the timer,
  // to avoid running the AutoRefreshTimer when we don't need it.
  //refresh_timer = std::make_unique<AutoRefreshTimer>();
  timer_displayed = false;
  timer_running = true;
  // TODO(#11):  Hard coded 5 minutes right now
  timer_end = std::chrono::duration_cast<std::chrono::seconds>(
                  std::chrono::system_clock::now().time_since_epoch()) +
              std::chrono::minutes(5);  // NOLINT(readability-magic-numbers)
}

auto TimerManager::displayTime() -> std::string {
  std::chrono::seconds time_left = timeLeft();

  std::chrono::minutes minutes_left =
      std::chrono::duration_cast<std::chrono::minutes>(time_left);
  std::chrono::seconds seconds_left = time_left - minutes_left;

  return StringUtil::intToString(minutes_left.count()) + ":" +
         StringUtil::intToString(seconds_left.count(), 2);
}

auto TimerManager::timeLeft() -> std::chrono::seconds {
  // TODO(#11): Handle pausing the timer here
  std::chrono::seconds time_left =
      timer_end - std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::system_clock::now().time_since_epoch());
  if (time_left < std::chrono::seconds(0)) {
    return std::chrono::seconds(0);
  }
  return time_left;
}

}  // namespace cszb_scoreboard
