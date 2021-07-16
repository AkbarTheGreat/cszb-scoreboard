/*
ui/component/control/ScoreControl.h: This class is responsible for generating a
score update which is sent to all scoreboard screens.

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

#include <memory>    // for unique_ptr
#include <optional>  // for optional
#include <vector>    // for vector

#include "config/swx/image.h"
#include "ui/component/control/ScreenTextController.h"  // for ScreenTextCon...
#include "ui/widget/Button.h"                           // for Button
#include "ui/widget/ColorPicker.h"                      // for ColorPicker
#include "ui/widget/Label.h"                            // for Label
#include "ui/widget/Panel.h"                            // for Panel
#include "ui/widget/Text.h"                             // for Text
#include "ui/widget/Toggle.h"                           // for Toggle

namespace cszb_scoreboard {
class PreviewPanel;
class ScreenText;
class Widget;

namespace proto {
class RenderableText;
}  // namespace proto
namespace swx {
class Panel;
}  // namespace swx

class ScoreControl : public ScreenTextController {
 public:
  ScoreControl(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenTextController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<ScoreControl>;

 private:
  void addHomeAwayWidgetPair(Panel *panel, int row, const Widget &home_widget,
                             const Widget &away_widget);
  void addToEntry(Text *entry, int amount);
  void bindEvents();
  void createControls(Panel *control_panel) override;
  void positionWidgets(Panel *control_panel);
  auto introLines(bool isHome) -> std::vector<proto::RenderableText>;
  auto scoreLines(bool isHome) -> std::vector<proto::RenderableText>;
  void updateScreenText(ScreenText *screen_text) override;
  void awayUpdated();
  void awayNameUpdated();
  void awayAddOne();
  void awayAddFive();
  void awayMinusOne();
  void colorChanged();
  void homeUpdated();
  void homeNameUpdated();
  void homeAddOne();
  void homeAddFive();
  void homeMinusOne();
  void selectLogo(bool isHome);
  void toggleIntroMode();

  std::optional<Image> home_logo, away_logo;
  std::unique_ptr<Text> alpha_ctrl, size_ctrl;
  std::unique_ptr<Toggle> team_intro_button;
  std::unique_ptr<Panel> team_controls_panel;
  std::unique_ptr<Label> home_score_label, away_score_label;
  std::unique_ptr<ColorPicker> home_color_picker, away_color_picker;
  std::unique_ptr<Text> home_name_entry, away_name_entry;
  std::unique_ptr<Text> home_score_entry, away_score_entry;
  std::unique_ptr<Panel> home_button_panel, away_button_panel;
  std::unique_ptr<Button> home_plus_1, home_plus_5, home_minus_1;
  std::unique_ptr<Button> away_plus_1, away_plus_5, away_minus_1;
  std::unique_ptr<Button> home_logo_button, away_logo_button;
  std::unique_ptr<Label> home_logo_label, away_logo_label;
};

}  // namespace cszb_scoreboard
