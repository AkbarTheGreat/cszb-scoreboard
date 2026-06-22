/*
ui/component/control/things_mode/SingleTeamActivityPanel.cpp: Represents all
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

#include "ui/component/control/things_mode/SingleTeamActivityPanel.h"

#include <algorithm>  // for max
#include <string>     // for char_...

#include "ui/component/control/ScreenTextController.h"          // for Scree...
#include "ui/component/control/things_mode/Activity.h"          // for Activity
#include "ui/component/control/things_mode/ReplacementPanel.h"  // for Repla...
#include "ui/graphics/TeamColors.h"                             // for TeamC...
#include "util/ProtoUtil.h"                                     // for Proto...
#include "wx/clrpicker.h"                                       // for wxCol...

namespace cszb_scoreboard {

static const char* BULLET = "\u2022";

SingleTeamActivityPanel::SingleTeamActivityPanel(
    swx::Panel* wx, ScreenTextController* owning_controller,
    const proto::ScreenSide& side, Singleton* singleton)
    : ActivityPanel(wx, owning_controller, singleton) {
  this->side = side;
  this->singleton = singleton;
  color_picker = colorPicker(singleton->teamColors()->getColor(side));
  bindEvents();
  positionWidgets();
}

void SingleTeamActivityPanel::bindEvents() {
  color_picker->bind(
      wxEVT_COLOURPICKER_CHANGED,
      [this](wxColourPickerEvent& event) -> void { this->colorChanged(); });
}

void SingleTeamActivityPanel::positionWidgets() {
  addWidget(*color_picker, 1, 0);
  runSizer();
}

void SingleTeamActivityPanel::colorChanged() {
  singleton->teamColors()->setColor(side, color_picker->color());

  owning_controller->updatePreview();
}

auto SingleTeamActivityPanel::activityText(const proto::ScreenSide& side,
                                           int font_size)
    -> std::vector<proto::RenderableText> {
  std::string preview_text;
  for (const auto& activity : activities) {
    preview_text += std::string(BULLET) + " " + activity->previewText() + "\n";
  }
  std::vector<proto::RenderableText> return_vector;
  return_vector.emplace_back();
  return_vector.back().set_text(preview_text);
  return_vector.back().mutable_font()->set_size(font_size);
  return return_vector;
}

auto SingleTeamActivityPanel::color(const proto::ScreenSide& side) -> Color {
  color_picker->setColor(singleton->teamColors()->getColor(side));
  return singleton->teamColors()->getColor(side);
}

auto SingleTeamActivityPanel::replacementColor() -> Color {
  return color(ProtoUtil::noSide());
}

auto SingleTeamActivityPanel::replacementText(const proto::ScreenSide& side,
                                              int font_size)
    -> std::vector<proto::RenderableText> {
  return replacementPanel()->previewText(font_size);
}

}  // namespace cszb_scoreboard