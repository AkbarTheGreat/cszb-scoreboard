/*
ui/component/control/things_mode/ActivityPanel.h: Represents all activities in
5/6 things.

Copyright 2019-2021 Tracy Beck

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

#include <wx/clrpicker.h>
#include <wx/wx.h>

#include <vector>

#include "config.pb.h"
#include "ui/component/control/things_mode/Activity.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {
class ActivityPanel : public Panel {
 public:
  ActivityPanel(swx::Panel *wx, ScreenTextController *owning_controller,
                const proto::ScreenSide &side);
  void addActivity(wxPanel *parent_panel);
  void addReplacement();
  auto getColor() -> Color;
  auto previewText(int font_size) -> std::vector<proto::RenderableText>;
  void refreshSizers();
  auto replacementPanel() -> ReplacementPanel *;
  auto selectedActivityText() -> std::string;
  void swapActivities(int a, int b);
  void updateNotify();
  // wxWidgets callbacks, waive linting error for references.
  void deleteActivity(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void selectionChanged(
      wxCommandEvent &event);           // NOLINT(google-runtime-references)
  void textUpdated(wxKeyEvent &event);  // NOLINT(google-runtime-references)

 private:
  std::unique_ptr<Panel> activity_half, replacement_half;
  wxColourPickerCtrl *color_picker;
  proto::ScreenSide side;
  std::vector<std::shared_ptr<Activity>> activities;
  ScreenTextController *owning_controller;
  wxWindow *parent;

  void bindEvents();
  void positionWidgets();
  void colorChanged(
      wxColourPickerEvent &event);  // NOLINT(google-runtime-references)
                                    // wxWidgets callback.
  void resetActivityMoveButtons();
};
}  // namespace cszb_scoreboard
