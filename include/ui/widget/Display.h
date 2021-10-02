/*
ui/widget/Display.h: A representation of a physical display.  This is notable
within the widgets classes in that it owns it's underlying wx object and
construct it itself in normal cases, and has an injectible constructor for
testing.

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

#include "ScoreboardCommon.h"
#include "config.pb.h"  // for ScreenSide, Font (ptr only), Color (p...
#include "ui/widget/swx/Display.h"

namespace cszb_scoreboard {

class Display {
 public:
  Display(uint32_t index0);

  void geometry(proto::Rectangle* rectangle);
  auto isPrimary() -> bool { return wx->IsPrimary(); }

  static auto numDisplays() -> uint32_t { return wxDisplay::GetCount(); }

  PUBLIC_TEST_ONLY
  Display(std::shared_ptr<swx::Display> wx) { this->wx = wx; }

 private:
  // This is a shared pointer for testing.  Otherwise it could just be a
  // unique_ptr.
  std::shared_ptr<swx::Display> wx;
};

}  // namespace cszb_scoreboard
