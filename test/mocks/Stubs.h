/*
Stubs.h: Some miscellaneous stubs of methods that will normally be
present in the actual code that might be stubbed for unit tests.  To use, set
the defines in the test.cpp of the stubs you wish to enable, and then include
this file.  Similar to:

#define TEST_STUB_FRAME_MANAGER
#define TEST_STUB_MAIN_VIEW
#include "test/mocks/Stubs.h"

Copyright 2024-2025 Tracy Beck

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

#include "test/mocks/util/MockSingleton.h"
#include "ui/frame/FrameManager.h"
#include "ui/frame/MainView.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {

/* Stubbed methods from Singleton.h */
#ifdef TEST_STUB_SINGLETON
auto Singleton::getInstance() -> Singleton * {
  static test::MockSingleton instance;
  return &instance;
}
#endif  // #ifdef TEST_STUB_SINGLETON

/* Stubbed methods from FrameManager.h */
#ifdef TEST_STUB_FRAME_MANAGER
auto FrameManager::createMainView(const std::string &title, const Position &pos,
                                  const Size &size) -> MainView * {
  return nullptr;
}

auto FrameManager::createScreenPresenter(int monitor_number,
                                         const ScreenText &preview)
    -> ScreenPresenter * {
  return nullptr;
}

void FrameManager::refreshFrames() {}

void FrameManager::exitFrames() {}

auto FrameManager::monitor(uint32_t index) -> Display { return Display(0); }
#endif  // #ifdef TEST_STUB_FRAME_MANAGER

/* Stubbed methods from MainView.h */
#ifdef TEST_STUB_MAIN_VIEW
#include "ui/widget/Frame.h"
MainView::MainView(cszb_scoreboard::swx::Frame *wx,
                   cszb_scoreboard::Singleton *singleton)
    : Frame(wx) {}
void cszb_scoreboard::MainView::init() {};
#endif  // #ifdef TEST_STUB_MAIN_VIEW

/* Stubbed methods from PersistentTimer.h */
#ifdef TEST_STUB_PERSISTENT_TIMER

std::function<void()> last_on_tick;

PersistentTimer::PersistentTimer(int period,
                                 std::function<void()> const &on_tick) {
  last_on_tick = on_tick;
}

// This is a bit of a hack to simulate timers executing on their schedule for
// unit testing.  This method is not presently even implemented in the
// production version of this class.
void PersistentTimer::trigger() { last_on_tick(); }

#endif  // #ifdef TEST_STUB_PERSISTENT_TIMER

}  // namespace cszb_scoreboard
