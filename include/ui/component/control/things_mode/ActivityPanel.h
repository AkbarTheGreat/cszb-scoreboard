/*
ui/component/control/things_mode/ActivityPanel.h: Represents all activities in
5/6 things.

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

#include <memory>  // for unique_ptr
#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"
#include "config.pb.h"                                  // for RenderableTex...
#include "ui/component/control/things_mode/Activity.h"  // for Activity
#include "ui/graphics/Color.h"                          // for Color
#include "ui/widget/ColorPicker.h"                      // for ColorPicker
#include "ui/widget/LabelledArea.h"
#include "ui/widget/Panel.h"  // for Panel
#include "util/Singleton.h"

namespace cszb_scoreboard {
class ReplacementPanel;
class ScreenTextController;

namespace swx {
class Panel;
}  // namespace swx

class ActivityPanel : public Panel {
 public:
  ActivityPanel(swx::Panel *wx, ScreenTextController *owning_controller,
                const proto::ScreenSide &side)
      : ActivityPanel(wx, owning_controller, side, Singleton::getInstance()) {}
  void addActivity();
  void addReplacement();
  auto getColor() -> Color;
  auto previewText(int font_size) -> std::vector<proto::RenderableText>;
  void refreshSizers();
  auto replacementPanel() -> ReplacementPanel *;
  auto selectedActivityText() -> std::string;
  void swapActivities(int a, int b);
  void updateNotify();
  void deleteActivity(Activity *deleted);
  void selectionChanged(Activity *selected);
  void textUpdated();

  PUBLIC_TEST_ONLY
  ActivityPanel(swx::Panel *wx, ScreenTextController *owning_controller,
                const proto::ScreenSide &side, Singleton *singleton);

 private:
  std::unique_ptr<LabelledArea> activity_label, replacement_label;
  std::unique_ptr<Panel> activity_half, replacement_half;
  std::unique_ptr<ColorPicker> color_picker;
  proto::ScreenSide side;
  std::vector<std::unique_ptr<Activity>> activities;
  ScreenTextController *owning_controller;
  Singleton *singleton;

  void bindEvents();
  void positionWidgets();
  void colorChanged();
  void resetActivityMoveButtons();
  void hideAllReplacements();
  void showReplacement(int index);
  void showSelectedReplacement();
};
}  // namespace cszb_scoreboard
