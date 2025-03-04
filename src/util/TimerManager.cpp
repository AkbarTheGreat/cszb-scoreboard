/*
util/TimerManager.cpp: Singleton which handles the current time left on the game
timer.

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

#include "util/TimerManager.h"

#include <compare>            // for operator<, strong_ordering
#include <ratio>              // for ratio
#include <memory>             // for allocator

#include "util/StringUtil.h"  // for StringUtil

namespace cszb_scoreboard {

TimerManager::TimerManager(SingletonClass c, Singleton *singleton) {
  timer_displayed = false;
  timer_running = false;
  this->singleton = singleton;
  time_left = std::chrono::minutes(
      1);  // Just a valid default value, unimportant what it is.
  timer_end = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch());
}

void TimerManager::setTime(int64_t seconds) {
  auto new_time_left = std::chrono::seconds(seconds);
  timer_end = std::chrono::duration_cast<std::chrono::seconds>(
                  std::chrono::system_clock::now().time_since_epoch()) +
              new_time_left;
  time_left = new_time_left;
}

void TimerManager::startTimer() {
  if (!timer_running) {
    timer_running = true;
    timer_end = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()) +
                time_left;
  }
}

void TimerManager::pauseTimer() { timer_running = false; }

void TimerManager::showTimer() {
  if (!timer_displayed) {
    timer_displayed = true;
    singleton->autoRefreshTimer();
  }
}

void TimerManager::hideTimer() {
  if (timer_displayed) {
    timer_displayed = false;
    // We used to stop the autorefresh timer when this was the only client.  Now
    // we either just let it run indefinitely or we should have a use counter
    // going.
    // refresh_timer->stop();
    // refresh_timer.reset();
  }
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
  if (timer_running) {
    time_left =
        timer_end - std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch());
    if (time_left < std::chrono::seconds(0)) {
      time_left = std::chrono::seconds(0);
    }
  }
  return time_left;
}

}  // namespace cszb_scoreboard
