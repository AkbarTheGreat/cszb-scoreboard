/*
ui/component/control/things_mode/HeadToHeadActivityPanel.cpp: Represents all
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

#include "ui/component/control/things_mode/HeadToHeadActivityPanel.h"

#include <algorithm>  // for max
#include <string>     // for char_...

#include "config.pb.h"                                          // for Rende...
#include "ui/component/control/ScreenTextController.h"          // for Scree...
#include "ui/component/control/things_mode/Activity.h"          // for Activity
#include "ui/component/control/things_mode/ReplacementPanel.h"  // for Repla...
#include "ui/graphics/TeamColors.h"                             // for TeamC...
#include "util/ProtoUtil.h"                                     // for Proto...
#include "wx/clrpicker.h"                                       // for wxCol...

namespace cszb_scoreboard {

static const char* BULLET = "\u2022";

HeadToHeadActivityPanel::HeadToHeadActivityPanel(
    swx::Panel* wx, ScreenTextController* owning_controller,
    Singleton* singleton)
    : ActivityPanel(wx, owning_controller, singleton) {
  this->singleton = singleton;
  away_color_picker =
      colorPicker(singleton->teamColors()->getColor(ProtoUtil::awaySide()));
  home_color_picker =
      colorPicker(singleton->teamColors()->getColor(ProtoUtil::homeSide()));

  bool is_home = true;
  for (const auto& activity : activities) {
    activity->setTeam(is_home);
    is_home = !is_home;
  }

  bindEvents();
  positionWidgets();
}

void HeadToHeadActivityPanel::bindEvents() {
  away_color_picker->bind(
      wxEVT_COLOURPICKER_CHANGED,
      [this](wxColourPickerEvent& event) { awayColorChanged(); });
  home_color_picker->bind(
      wxEVT_COLOURPICKER_CHANGED,
      [this](wxColourPickerEvent& event) { homeColorChanged(); });
}

void HeadToHeadActivityPanel::positionWidgets() {
  addWidget(*home_color_picker, 1, 0);
  addWidget(*away_color_picker, 1, 1);
  runSizer();
  refreshSizers();
}

auto HeadToHeadActivityPanel::activityText(const proto::ScreenSide& side,
                                           int font_size)
    -> std::vector<proto::RenderableText> {
  std::string preview_text;
  for (const auto& activity : activities) {
    if ((side.home() && activity->getTeam().home()) ||
        (side.away() && activity->getTeam().away())) {
      preview_text +=
          std::string(BULLET) + " " + activity->previewText() + "\n";
    }
  }
  std::vector<proto::RenderableText> return_vector;
  return_vector.emplace_back();
  return_vector.back().set_text(preview_text);
  return_vector.back().mutable_font()->set_size(font_size);
  return return_vector;
}

auto HeadToHeadActivityPanel::color(const proto::ScreenSide& side,
                                    bool forReplacement) -> Color {
  if (forReplacement) {
    for (const auto& activity : activities) {
      if (activity->isSelected()) {
        if (activity->getTeam().away()) {
          return away_color_picker->color();
        }
        return home_color_picker->color();
      }
    }
  }
  if (side.home()) {
    return home_color_picker->color();
  }
  return away_color_picker->color();
}

auto HeadToHeadActivityPanel::replacementText(const proto::ScreenSide& side,
                                              int font_size)
    -> std::vector<proto::RenderableText> {
  return replacementPanel()->previewText(font_size);
}

void HeadToHeadActivityPanel::setTeamForNewActivity(int index) {
  if (index < 0 || index >= static_cast<int>(activities.size())) {
    return;
  }

  if (index == 0) {
    activities[index]->setTeam(true);
    return;
  }

  activities[index]->setTeam(!activities[index - 1]->getTeam().home());
}

// We split screens for head-to-head activities, but not for replacements.
auto HeadToHeadActivityPanel::splitScreens(bool forReplacement) -> bool {
  return !forReplacement;
}

void HeadToHeadActivityPanel::awayColorChanged() {
  singleton->teamColors()->setColor(ProtoUtil::awaySide(),
                                    away_color_picker->color());

  owning_controller->updatePreview();
}

void HeadToHeadActivityPanel::homeColorChanged() {
  singleton->teamColors()->setColor(ProtoUtil::homeSide(),
                                    home_color_picker->color());

  owning_controller->updatePreview();
}

}  // namespace cszb_scoreboard