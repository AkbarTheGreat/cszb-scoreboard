/*
ui/ScreenPresenter.h: This class manages any content which is being
displayed on an external screen.

Copyright 2019-2024 Tracy Beck

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

#include <memory>  // for unique_ptr

#include "ScoreboardCommon.h"
#include "ui/component/ScreenText.h"  // for ScreenText
#include "ui/widget/Frame.h"          // for Frame
#include "util/Singleton.h"

namespace cszb_scoreboard {
class ScreenPresenter : public Frame {
 public:
  ScreenPresenter(int monitor_number, const ScreenText &preview)
      : ScreenPresenter(monitor_number, preview, Singleton::getInstance()) {}
  void blackout() { screen_text->blackout(); }
  void setAll(const ScreenText &source) { screen_text->setAll(source); }

  PUBLIC_TEST_ONLY
  ScreenPresenter(int monitor_number, const ScreenText &preview,
                  Singleton *singleton);

 private:
  int monitor_number;
  std::unique_ptr<ScreenText> screen_text;
};

}  // namespace cszb_scoreboard
