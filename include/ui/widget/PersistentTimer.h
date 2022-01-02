/*
ui/widget/PersistentTimer.h: A timer object which performs a given action
periodically.

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

#include <functional>  // for function

#include "ui/widget/swx/Timer.h"  // for Timer

namespace cszb_scoreboard {

class PersistentTimer {
 private:
  class HeldTimer : public swx::Timer {
   public:
    explicit HeldTimer(const std::function<void()> &on_tick);

   private:
    void Notify() override;
    std::function<void()> on_tick;
  };

 public:
  PersistentTimer(int period, const std::function<void()> &on_tick);

 private:
  // We should delete the held timer at destruction, but doing so causes a read
  // exception on Windows, so we just leak it at present.  Since all of the
  // timers we have right now last until the application closes, we can live
  // with this (unfortunately).
  HeldTimer *held;
};

}  // namespace cszb_scoreboard
