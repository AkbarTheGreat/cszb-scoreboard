/*
ui/component/control/ScreenImageController.h: A specialized ScreenTextController
which handles sending images loaded from elsewhere, as opposed to a static
background.  By default, this also does not support any text at all.

Copyright 2020-2025 Tracy Beck

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

#include "ui/component/control/ScreenImageController.h"

#include "ScoreboardCommon.h"
#include "config/swx/event.h"
#include "ui/component/ScreenText.h"
#include "ui/graphics/Color.h"
#include "ui/widget/Panel.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

//  This string has a bunch of trailing whitespace to buffer out the
//  layout to make room for most image names.  It's hacky, but it's a bit
//  better than completely resizing the panels, IMHO.
const std::string NO_IMAGE_MESSAGE =
    "<No Image Selected>                               ";

void ScreenImageController::createControls(Panel *control_panel) {
  screen_selection =
      std::make_unique<TeamSelector>(childPanel(), ProtoUtil::allSide());
  current_image_label = control_panel->label(NO_IMAGE_MESSAGE);
  bindEvents();
}

void ScreenImageController::bindEvents() {
  screen_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent &event) -> void { this->screenChanged(); });
}

void ScreenImageController::updateScreenText(ScreenText *screen_text) {
  if (!isActive()) {
    return;
  }
  if (screen_selection->allSelected() && all_screen_image.IsOk()) {
    // Send the image to both screens
    screen_text->setAllText("", 1, Color("Black"), false, ProtoUtil::allSide());
    screen_text->setImage(all_screen_image, ProtoUtil::allSide());
  } else {
    if (home_screen_image.IsOk()) {
      screen_text->setAllText("", 1, Color("Black"), false,
                              ProtoUtil::homeSide());
      screen_text->setImage(home_screen_image, ProtoUtil::homeSide());
    }
    if (away_screen_image.IsOk()) {
      screen_text->setAllText("", 1, Color("Black"), false,
                              ProtoUtil::awaySide());
      screen_text->setImage(away_screen_image, ProtoUtil::awaySide());
    }
  }
}

void ScreenImageController::screenChanged() {
  if (screen_selection->allSelected()) {
    if (all_screen_image_name.empty()) {
      current_image_label->set(NO_IMAGE_MESSAGE);
    } else {
      current_image_label->set(all_screen_image_name);
    }
  } else if (screen_selection->homeSelected()) {
    if (home_screen_image_name.empty()) {
      current_image_label->set(NO_IMAGE_MESSAGE);
    } else {
      current_image_label->set(home_screen_image_name);
    }
  } else if (screen_selection->awaySelected()) {
    if (away_screen_image_name.empty()) {
      current_image_label->set(NO_IMAGE_MESSAGE);
    } else {
      current_image_label->set(away_screen_image_name);
    }
  }

  control_panel->update();
  updatePreview();
}

}  // namespace cszb_scoreboard
