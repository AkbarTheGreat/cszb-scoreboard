/*
ui/event/UpdateTimer.cpp: A timer which periodically checks for updates.

Copyright 2019-2022 Tracy Beck

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

#include "ui/event/UpdateTimer.h"

#include <functional>  // for function

#include "ScoreboardCommon.h"           // for SCOREBOARD_VERSION
#include "ui/widget/Frame.h"            // for Frame
#include "ui/widget/PersistentTimer.h"  // for PersistentTimer
#include "util/AutoUpdate.h"            // for AutoUpdate

namespace cszb_scoreboard {

// Retry every six hours to look for an update.
constexpr int AUTO_UPDATE_DELAY = 6 * 60 * 60 * 1000;

UpdateTimer::UpdateTimer(Frame *main_view, Singleton *singleton)
    : PersistentTimer(AUTO_UPDATE_DELAY,
                      [this]() -> void { this->execute(); }) {
  this->main_view = main_view;
  this->singleton = singleton;
  // First time through, remove an old auto-update.
  singleton->autoUpdate()->removeOldUpdate();
  execute();
}

void UpdateTimer::execute() {
  bool new_version_available =
      singleton->autoUpdate()->checkForUpdate(SCOREBOARD_VERSION);
  if (new_version_available) {
    if (singleton->autoUpdate()->updateIsDownloadable()) {
      main_view->setStatusBar("New version found, downloading...");
      if (singleton->autoUpdate()->updateInPlace()) {
        main_view->setStatusBar(
            "Auto-update downloaded.  Please restart to apply.");
      } else {
        main_view->setStatusBar(
            "Auto-update failed!  Please manually update scoreboard.");
      }
    } else {
      main_view->setStatusBar(
          "New version found, please go to "
          "github.com/AkbarTheGreat/cszb-scoreboard to update.");
    }
  }
}

}  // namespace cszb_scoreboard
