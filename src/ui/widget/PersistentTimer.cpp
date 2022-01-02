/*
ui/widget/PersistentTimer.cpp: A timer object which performs a given action
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

#include "ui/widget/PersistentTimer.h"

namespace cszb_scoreboard {

PersistentTimer::PersistentTimer(int period,
                                 const std::function<void()>& on_tick) {
  held = new HeldTimer(on_tick);
  held->Start(period, false);
}

PersistentTimer::~PersistentTimer() { delete held; }

PersistentTimer::HeldTimer::HeldTimer(const std::function<void()>& on_tick) {
  this->on_tick = on_tick;
}

void PersistentTimer::HeldTimer::Notify() { on_tick(); }

}  // namespace cszb_scoreboard
