/*
util/TimerManager.cpp: Singleton which handles the current time left on the game
timer.

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

#include "util/TimerManager.h"

namespace cszb_scoreboard {

TimerManager::TimerManager(SingletonClass c, Singleton *singleton) {
  refresh_timer = std::make_unique<AutoRefreshTimer>();
}

}  // namespace cszb_scoreboard
