/*
ui/component/control/TextEntry.h: This class is responsible for generating text
which can go to one or all of the scoreboard screens.

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

#include <wx/wx.h>

#include "ScoreboardCommon.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"
#include "ui/widget/ColorPicker.h"
#include "ui/widget/Label.h"
#include "ui/widget/Panel.h"
#include "ui/widget/Text.h"

namespace cszb_scoreboard {

class TextEntry : public ScreenTextController {
 public:
  TextEntry(PreviewPanel *preview_panel, swx::Panel *wx);
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<TextEntry>;
  auto textField() -> Text *;
  void textUpdated();

PUBLIC_TEST_ONLY
  void selectTeam(int index);

 private:
  std::unique_ptr<ColorPicker> color_picker;
  std::unique_ptr<Label> font_size_label, text_label;
  std::unique_ptr<Panel> inner_panel;
  std::unique_ptr<Text> font_size_entry, text_entry;
  std::unique_ptr<TeamSelector> screen_selection;
  std::string home_text, away_text, all_text;
  int home_font_size, away_font_size, all_font_size;
  Color home_color, away_color, all_color;

  void updateScreenText(ScreenText *screen_text) override;
  void createControls(Panel *control_panel) override;

  void bindEvents();
  auto enteredFontSize() -> int;
  void positionWidgets(Panel *control_panel);
  void screenChanged();
  // wxWidgets callbacks, waive linting error for references.
  void colorChanged(const wxColourPickerEvent &event);
};

}  // namespace cszb_scoreboard
