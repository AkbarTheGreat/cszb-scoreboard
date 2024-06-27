/*
ui/event/AutoRefreshTimer.h: A timer which forces all of the frames to repaint
periodically.

Copyright 2021-2024 Tracy Beck

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

#include <ui/widget/PersistentTimer.h>  // for PersistentTimer

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "util/Singleton.h"    // for Singleton

namespace cszb_scoreboard {

class AutoRefreshTimer : public PersistentTimer {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  AutoRefreshTimer() : AutoRefreshTimer(Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP

  PUBLIC_TEST_ONLY
  explicit AutoRefreshTimer(Singleton* singleton);

 private:
  void execute();
  Singleton* singleton;
};

}  // namespace cszb_scoreboard