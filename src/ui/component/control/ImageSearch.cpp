/*
ui/component/control/ImageSearch.h: Handles searching for images from a webview
for quick use as a bit.

Copyright 2022 Tracy Beck

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

#include "ui/component/control/ImageSearch.h"

#include <filesystem>  // for path
#include <optional>    // for optional
#include <string>      // for string

#include "ScoreboardCommon.h"                   // for DEFAULT_BORDER_SIZE
#include "config/swx/event.h"                   // for wxEVT_BUTTON
#include "config/swx/image.h"                   // for Image
#include "ui/component/control/TeamSelector.h"  // for TeamSelector
#include "ui/widget/FilePicker.h"               // for FilePicker
#include "ui/widget/Label.h"                    // for Label
#include "ui/widget/PopUp.h"                    // for PopUp
#include "ui/widget/Widget.h"                   // for NO_BORDER
#include "util/Clipboard.h"                     // for Clipboard
#include "util/FilesystemPath.h"                // for FilesystemPath

namespace cszb_scoreboard {

class PreviewPanel;

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const std::string DROP_MESSAGE = "<Drop Image Here To Load>";
const Size DROP_TARGET_SIZE{.width = 120, .height = 360};
const Size BROWSER_SIZE{.width = 720, .height = 360};
const int DROP_TARGET_BORDER = 50;

auto ImageSearch::Create(PreviewPanel *preview_panel, swx::Panel *wx)
    -> std::unique_ptr<ImageSearch> {
  auto local_image = std::make_unique<ImageSearch>(preview_panel, wx);
  local_image->initializeWidgets();
  local_image->updatePreview();
  return local_image;
}

void ImageSearch::createControls(Panel *control_panel) {
  ScreenImageController::createControls(control_panel);
  drop_target = control_panel->panel();
  drop_target->setBorder();
  drop_text = drop_target->label(DROP_MESSAGE);
  browser = control_panel->browser("https://images.google.com");
  reset_button_panel = control_panel->panel();
  reset_button = reset_button_panel->button("Reset\nBrowser", true);

  all_screen_image_name = "";
  away_screen_image_name = "";
  home_screen_image_name = "";
  current_image_label->set("");

  positionWidgets(control_panel);
  bindEvents();
}

void ImageSearch::positionWidgets(Panel *control_panel) {
  drop_target->addWidget(*drop_text, 0, 0, DROP_TARGET_SIZE.height / 2, wxTOP);
  reset_button_panel->addWidget(*reset_button, 0, 0, NO_BORDER);

  control_panel->addWidgetWithSpan(*drop_target, 0, 0, 2, 1, DROP_TARGET_SIZE,
                                   NO_BORDER);

  control_panel->addWidget(*screen_selection, 0, 1, NO_BORDER);
  control_panel->addWidget(*reset_button_panel, 1, 1, NO_BORDER);

  control_panel->addWidgetWithSpan(*browser, 0, 2, 2, 1, BROWSER_SIZE,
                                   NO_BORDER);

  // I have to put this empty label somewhere to be compliant, so I just shove
  // it at the bottom.
  control_panel->addWidget(*current_image_label, 2, 0);

  drop_target->runSizer();
  control_panel->runSizer();
}

void ImageSearch::bindEvents() {
  drag_handler = std::make_unique<DragAndDropHandler>(
      drop_target.get(),
      [](int32_t x, int32_t y, const std::string &url) -> void {
        onURLDrop(url);
      });
}

void ImageSearch::onURLDrop(const std::string &url) {
  wxLogDebug("onURLDrop: %s", url);
}

}  // namespace cszb_scoreboard
