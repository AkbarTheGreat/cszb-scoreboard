/*
ui/event/UpdateTimer.cpp: A timer which periodically checks for updates.

Copyright 2019 Tracy Beck

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

#include "ScoreboardCommon.h"
#include "util/AutoUpdate.h"

namespace cszb_scoreboard {

// Retry every six hours to look for an update.
const int AUTO_UPDATE_DELAY = 6 * 60 * 60 * 1000;

UpdateTimer::UpdateTimer(wxFrame *main_view) : wxTimer() {
  this->main_view = main_view;
}

void UpdateTimer::Notify() {
  std::string new_version =
      AutoUpdate::getInstance()->checkForUpdate(SCOREBOARD_VERSION);
  if (new_version != "") {
    wxString string;
    string.Printf(wxT("New Version Found!  Please download at: %s"),
                  new_version);
    main_view->SetStatusText(string);
  }

  // If called as a one-shot, we need to establish this as a periodic event.
  if (IsOneShot()) {
    Start(AUTO_UPDATE_DELAY);
  }
}

}  // namespace cszb_scoreboard