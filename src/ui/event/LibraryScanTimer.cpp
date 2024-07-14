/*
ui/event/LibraryScanTimer.cpp: A timer which periodically rescans the image
library root for changes.

Copyright 2024 Tracy Beck

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

#include "ui/event/LibraryScanTimer.h"

#include <functional>  // for function

#include "config/ImageLibrary.h"
#include "ui/widget/Frame.h"            // for Frame
#include "ui/widget/PersistentTimer.h"  // for PersistentTimer

namespace cszb_scoreboard {

constexpr int REFRESH_RATE_MILLIS = 5 * 60 * 1000;  // 5 minute refresh

LibraryScanTimer::LibraryScanTimer(Frame* main_view, Singleton* singleton)
    : PersistentTimer(REFRESH_RATE_MILLIS,
                      [this]() -> void { this->execute(); }) {
  this->main_view = main_view;
  this->singleton = singleton;
}

void LibraryScanTimer::execute() {
  LibraryUpdateResults results =
      singleton->imageLibrary()->detectLibraryChanges();
  if (!results.addedImages().empty() || !results.movedImages().empty() ||
      !results.removedImages().empty()) {
    std::string message = "Library changes detected!";
    if (results.addedImages().size()) {
      message +=
          " Added " + std::to_string(results.addedImages().size()) + " images.";
    }
    if (results.movedImages().size()) {
      message +=
          " Moved " + std::to_string(results.movedImages().size()) + " images.";
    }
    if (results.removedImages().size()) {
      message += " Removed " + std::to_string(results.removedImages().size()) +
                 " images.";
    }
    main_view->setStatusBar(message);
  }
}

}  // namespace cszb_scoreboard
