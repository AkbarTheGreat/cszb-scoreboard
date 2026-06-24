/*
ui/component/control/things_mode/ActivityPanel.h: Represents all activities in
5/6 things.

Copyright 2019-2026 Tracy Beck

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
#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"                           // for PUBLIC_TEST_ONLY
#include "ui/component/control/things_mode/Activity.h"  // for Activity
#include "ui/graphics/Color.h"                          // for Color
#include "ui/widget/LabelledArea.h"                     // for LabelledArea
#include "ui/widget/Panel.h"                            // for Panel
#include "util/Singleton.h"                             // for Singleton

namespace cszb_scoreboard {
class ReplacementPanel;
class ScreenTextController;
namespace proto {
class RenderableText;
class ScreenSide;
}  // namespace proto

namespace swx {
class Panel;
}  // namespace swx

class ActivityPanel : public Panel {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  ActivityPanel(swx::Panel* wx, ScreenTextController* owning_controller)
      : ActivityPanel(wx, owning_controller, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void addActivity();
  void addReplacement();
  void refreshSizers();
  auto replacementPanel() -> ReplacementPanel*;
  auto selectedActivityText() -> std::string;
  void swapActivities(int a, int b);
  void updateNotify();
  void deleteActivity(Activity* deleted);
  void selectionChanged(Activity* selected);
  void textUpdated();

  virtual auto activityText(const proto::ScreenSide& side, int font_size)
      -> std::vector<proto::RenderableText> = 0;
  virtual auto color(const proto::ScreenSide& side, bool forReplacement)
      -> Color = 0;
  virtual auto replacementText(const proto::ScreenSide& side, int font_size)
      -> std::vector<proto::RenderableText> = 0;
  virtual void setTeamForNewActivity(int index){};
  // If the screen is not split, we presume to use whatever is returned as the
  // "home" color and lines for everything.
  virtual auto splitScreens(bool forReplacement) -> bool = 0;

  PUBLIC_TEST_ONLY
  ActivityPanel(swx::Panel* wx, ScreenTextController* owning_controller,
                Singleton* singleton);

 protected:
  ScreenTextController* owning_controller;
  std::vector<std::unique_ptr<Activity>> activities;

 private:
  std::unique_ptr<LabelledArea> activity_label, replacement_label;
  std::unique_ptr<Panel> activity_half, replacement_half;
  Singleton* singleton;

  void bindEvents();
  void positionWidgets();
  void resetActivityMoveButtons();
  void hideAllReplacements();
  void showReplacement(int index);
  void showSelectedReplacement();
};
}  // namespace cszb_scoreboard
