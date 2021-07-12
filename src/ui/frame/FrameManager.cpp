/*
ui/frame/FrameManager.cpp: This class is a singleton which manages all top
level frames (or windows) in the current application.  This class owns
all of these objects and any pointers returned from here should be considered
non-authoritative.  All frame pointers are destroyed on exitFrames and any
dangling references will then be invalid.

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

#include "ui/frame/FrameManager.h"

#include <utility>

#include "ui/component/ScreenPresenter.h"
#include "ui/widget/Frame.h"

namespace cszb_scoreboard {
class ScreenText;

auto FrameManager::createMainView(const wxString &title, const wxPoint &pos,
                                  const wxSize &size) -> MainView * {
  main_view = std::make_unique<MainView>(title, pos, size);
  return main_view.get();
}

auto FrameManager::createScreenPresenter(int monitor_number,
                                         const ScreenText &preview)
    -> ScreenPresenter * {
  auto presenter = std::make_unique<ScreenPresenter>(monitor_number, preview);
  ScreenPresenter *temporary = presenter.get();
  frames.emplace(std::move(presenter));
  return temporary;
}

auto FrameManager::getInstance() -> FrameManager * {
  static FrameManager singleton;
  return &singleton;
}

void FrameManager::exitFrames() {
  // Deletes all of the unique_ptrs to these frames, allowing them to be cleaned
  frames.clear();
  main_view.reset();
}

}  // namespace cszb_scoreboard
