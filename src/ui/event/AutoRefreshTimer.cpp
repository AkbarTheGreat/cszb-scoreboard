/*
ui/event/AutoRefreshTimer.cpp: A timer which forces all of the frames to repaint
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

#include "ui/event/AutoRefreshTimer.h"

#include "ui/frame/FrameManager.h"

namespace cszb_scoreboard {

constexpr int REFRESH_RATE_MILLIS = 1000;

AutoRefreshTimer::AutoRefreshTimer(Singleton* singleton)
    : PersistentTimer(REFRESH_RATE_MILLIS,
                      [this]() -> void { this->execute(); }) {
  this->singleton = singleton;
}

void AutoRefreshTimer::execute() {
  auto* main = singleton->frameManager()->mainView();
  if (main == nullptr) {
    return;
  }
  main->refreshWindow();
}

}  // namespace cszb_scoreboard
