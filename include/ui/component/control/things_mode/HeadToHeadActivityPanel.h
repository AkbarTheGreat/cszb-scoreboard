/*
ui/component/control/things_mode/HeadToHeadActivityPanel.h: Represents all
activities in 5/6 things when played by both teams at the same time.

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
#include "ui/component/control/things_mode/ActivityPanel.h"  // for Activity...
#include "ui/graphics/Color.h"                               // for Color
#include "ui/widget/ColorPicker.h"                           // for ColorPicker
#include "util/Singleton.h"                                  // for Singleton

namespace cszb_scoreboard {
class ScreenTextController;
namespace proto {
class RenderableText;
class ScreenSide;
}  // namespace proto

namespace swx {
class Panel;
}  // namespace swx

class HeadToHeadActivityPanel : public ActivityPanel {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  HeadToHeadActivityPanel(swx::Panel* wx,
                          ScreenTextController* owning_controller)
      : HeadToHeadActivityPanel(wx, owning_controller,
                                Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  virtual auto activityText(const proto::ScreenSide& side, int font_size)
      -> std::vector<proto::RenderableText>;
  virtual auto color(const proto::ScreenSide& side, bool forReplacement)
      -> Color;
  virtual auto replacementText(const proto::ScreenSide& side, int font_size)
      -> std::vector<proto::RenderableText>;
  virtual void setTeamForNewActivity(int index);
  virtual auto splitScreens(bool forReplacement) -> bool;

  PUBLIC_TEST_ONLY
  HeadToHeadActivityPanel(swx::Panel* wx,
                          ScreenTextController* owning_controller,
                          Singleton* singleton);

 private:
  std::unique_ptr<ColorPicker> away_color_picker, home_color_picker;
  Singleton* singleton;

  void bindEvents();
  void positionWidgets();
  void awayColorChanged();
  void homeColorChanged();
};
}  // namespace cszb_scoreboard
