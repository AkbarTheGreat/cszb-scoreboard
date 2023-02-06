/*
ui/event/UpdateTimer.h: A timer which periodically checks for updates.

Copyright 2019-2023 Tracy Beck

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

#include <ui/widget/PersistentTimer.h>

#include "ScoreboardCommon.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {

class Frame;

class UpdateTimer : public PersistentTimer {
 public:
  explicit UpdateTimer(Frame *main_view)
      : UpdateTimer(main_view, Singleton::getInstance()) {}

  PUBLIC_TEST_ONLY
  explicit UpdateTimer(Frame *main_view, Singleton *singleton);

 private:
  Frame *main_view;
  Singleton *singleton;

  void execute();
};

}  // namespace cszb_scoreboard