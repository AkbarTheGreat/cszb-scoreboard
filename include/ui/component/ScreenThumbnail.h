/*
ui/component/ScreenThumbnail.h: Represents a very small view of exactly what's
on the matching ScreenPresenter, in case there's not a great view from the booth
to all monitors.

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

#include "ScoreboardCommon.h"
#include "config/Position.h"
#include "ui/component/ScreenText.h"  // for ScreenText
#include "util/Singleton.h"

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

class ScreenThumbnail : public ScreenText {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  ScreenThumbnail(swx::Panel *wx, int monitor_number, const ScreenText &preview)
      : ScreenThumbnail(wx, monitor_number, preview, Singleton::getInstance()) {
  }
  // GCOVR_EXCL_STOP

  PUBLIC_TEST_ONLY
  ScreenThumbnail(swx::Panel *wx, int monitor_number, const ScreenText &preview,
                  Singleton *singleton);

 private:
  Singleton *singleton;
  auto thumbnailSize(int monitor_number) -> Size;
};
}  // namespace cszb_scoreboard
