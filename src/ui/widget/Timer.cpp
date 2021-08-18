/*
ui/widget/Timer.cpp: A timer object which performs a given action periodically.

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

#pragma once

#include "ui/widget/Timer.h"

namespace cszb_scoreboard {

void Timer::start() {
  execute();
  Start(periodMillis(), false);
}

void Timer::Notify() { execute(); }

}  // namespace cszb_scoreboard
