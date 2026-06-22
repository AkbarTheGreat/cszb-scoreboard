/*
ui/component/control/things_mode/SingleTeamActivityPanel.h: Represents all
activities in 5/6 things when played by one team for the entirety of the game.

Copyright 2026 Tracy Beck

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
#include <vector>  // for vector

#include "ScoreboardCommon.h"                                // for PUBLIC_T...
#include "config.pb.h"                                       // for Renderab...
#include "ui/component/control/things_mode/ActivityPanel.h"  // for Activity...
#include "ui/graphics/Color.h"                               // for Color
#include "ui/widget/ColorPicker.h"                           // for ColorPicker
#include "util/Singleton.h"                                  // for Singleton

namespace cszb_scoreboard {
class ScreenTextController;

namespace swx {
class Panel;
}  // namespace swx

class SingleTeamActivityPanel : public ActivityPanel {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  SingleTeamActivityPanel(swx::Panel* wx,
                          ScreenTextController* owning_controller,
                          const proto::ScreenSide& side)
      : SingleTeamActivityPanel(wx, owning_controller, side,
                                Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP

  PUBLIC_TEST_ONLY
  SingleTeamActivityPanel(swx::Panel* wx,
                          ScreenTextController* owning_controller,
                          const proto::ScreenSide& side, Singleton* singleton);

  virtual auto activityText(const proto::ScreenSide& side, int font_size)
      -> std::vector<proto::RenderableText>;
  virtual auto color(const proto::ScreenSide& side) -> Color;
  virtual auto replacementColor() -> Color;
  virtual auto replacementText(const proto::ScreenSide& side, int font_size)
      -> std::vector<proto::RenderableText>;
  virtual auto splitScreens() -> bool { return false; }

 private:
  std::unique_ptr<ColorPicker> color_picker;
  proto::ScreenSide side;
  Singleton* singleton;

  void bindEvents();
  void colorChanged();
  void positionWidgets();
};
}  // namespace cszb_scoreboard
