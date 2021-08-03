/*
ui/component/PreviewPanel.cpp: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

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
#include "ui/component/PreviewPanel.h"

#include <algorithm>  // for max
#include <cassert>    // for assert
#include <utility>    // for move

#include "config.pb.h"                   // for ScreenSide, DisplayInfo
#include "config/DisplayConfig.h"        // for DisplayConfig
#include "ui/component/ScreenPreview.h"  // for ScreenPreview
#include "ui/widget/swx/Panel.h"         // for Panel

class wxCommandEvent;

namespace cszb_scoreboard {

class ScreenText;

PreviewPanel::PreviewPanel(swx::Panel *wx) : DraggablePanel(wx) {
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    std::vector<proto::ScreenSide> sides;
    if (display_info.side().error()) {
      sides.emplace_back(proto::ScreenSide());
      sides.back().set_error(true);
    }
    if (display_info.side().home()) {
      sides.emplace_back(proto::ScreenSide());
      sides.back().set_home(true);
    }
    if (display_info.side().away()) {
      sides.emplace_back(proto::ScreenSide());
      sides.back().set_away(true);
    }
    if (!sides.empty()) {
      screens.emplace_back(std::move(std::make_unique<ScreenPreview>(
          childPanel(), sides, display_info.id())));
    }
  }

  positionWidgets();
}

void PreviewPanel::positionWidgets() {
  for (const auto &screen : screens) {
    addWidget(*screen);
  }
  update();
}

auto PreviewPanel::numPreviews() -> int { return screens.size(); }

auto PreviewPanel::preview(int index) -> ScreenPreview * {
  assert(index >= 0 && index < screens.size());
  return screens[index].get();
}

void PreviewPanel::forAllScreens(
    const std::function<void(ScreenPreview *)> &lambda) {
  for (const auto &preview : screens) {
    lambda(preview.get());
  }
}

void PreviewPanel::setToPresenters(ScreenText *screen_text) {
  forAllScreens([screen_text](ScreenPreview *preview) -> void {
    preview->sendToPresenter(screen_text);
  });
}

void PreviewPanel::updatePresenters() {
  forAllScreens(
      [](ScreenPreview *preview) -> void { preview->sendToPresenter(); });
}

void PreviewPanel::updatePreviewsFromSettings() {
  // Eventually, we may have the possibility for settings to reset where we now
  // have more/less screens than we did before.  When that happens, this method
  // needs to deal with that case, too.

  int screen_index = 0;
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    if (screen_index < screens.size() &&
        (display_info.side().error() || display_info.side().home() ||
         display_info.side().away())) {
      screens[screen_index++]->resetFromSettings(i);
    }
  }
}

void PreviewPanel::blackout() {
  forAllScreens(
      [](ScreenPreview *preview) -> void { preview->blackoutPresenter(); });
}

}  // namespace cszb_scoreboard
