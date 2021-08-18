/*
ui/widget/Timer.h: A timer object which performs a given action periodically.

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

#pragma once

#include "ui/widget/swx/Timer.h"

namespace cszb_scoreboard {

class Timer  : public swx::Timer {
 public:
  Timer() = default;
  virtual void start();
  /* The period between executions of the timer. */
  virtual auto periodMillis() -> int = 0;
  /* The code to execute every iteration of the timer. */
  virtual void execute() =0;
 private:
    void Notify() override;
};

}  // namespace cszb_scoreboard
