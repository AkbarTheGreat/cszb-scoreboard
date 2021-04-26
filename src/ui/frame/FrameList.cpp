/*
ui/frame/FrameList.cpp: This class is a singleton which holds a list to all
active frames (or windows) in the current application.  This is primarily to
manage destroying all frames when the application is shut down.

Copyright 2019-2021 Tracy Beck

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

#include "ui/frame/FrameList.h"

#include "util/Log.h"

namespace cszb_scoreboard {

auto FrameList::getInstance() -> FrameList * {
  static FrameList singleton;
  return &singleton;
}

void FrameList::exitFrames() {
  for (auto *frame : frames) {
    LogDebug(wxT("Destroying a frame"));
    frame->Destroy();
  }
}

}  // namespace cszb_scoreboard
