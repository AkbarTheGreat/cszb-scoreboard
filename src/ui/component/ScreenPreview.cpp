/*
ui/ScreenPreview.cpp: This class manages the thumbnail preview of a monitor
in the main window.  In addition, the preview owns the ScreenPresenter which
displays to the actual monitor, dispatching the updated view when necessary.

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
#include "ui/component/ScreenPreview.h"

#include "ScoreboardCommon.h"              // for DEFAULT_BORDER_SIZE
#include "config.pb.h"                     // for DisplayInfo, ScreenSide
#include "config/DisplayConfig.h"          // for DisplayConfig
#include "config/TeamConfig.h"             // for TeamConfig
#include "config/swx/defs.h"               // for wxALIGN_CENTER, wxALL, wxLEFT
#include "ui/component/ScreenPresenter.h"  // for ScreenPresenter
#include "ui/component/ScreenText.h"       // for ScreenText
#include "ui/frame/FrameManager.h"         // for FrameManager
#include "util/ProtoUtil.h"                // for ProtoUtil

namespace cszb_scoreboard {

const int BORDER_SIZE = 10;
const char *WELCOME_MESSAGE = "Hello";
const char *ERROR_MESSAGE = "NO\nSCREENS\nFOUND!";
const int PREVIEW_HEIGHT = 320;

ScreenPreview::ScreenPreview(swx::Panel *wx,
                             std::vector<proto::ScreenSide> sides,
                             int monitor_number)
    : Panel(wx) {
  std::string initial_text;
  if (sides[0].error()) {
    initial_text = ERROR_MESSAGE;
  } else {
    initial_text = WELCOME_MESSAGE;
  }

  screen_text = std::make_unique<ScreenText>(childPanel());
  screen_text->setupPreview(initial_text, sides, previewSize(monitor_number));

  thumbnail = std::make_unique<ScreenThumbnail>(childPanel(), monitor_number,
                                                *screen_text);
  if (!sides[0].error()) {
    presenter = FrameManager::getInstance()->createScreenPresenter(
        monitor_number, *screen_text);
  }

  positionWidgets();
}

void ScreenPreview::positionWidgets() {
  addWidget(*thumbnail, 0, 0, DEFAULT_BORDER_SIZE,
            wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER);
  addWidget(*screen_text, 1, 0, BORDER_SIZE, wxALL);
  runSizer();
}

auto ScreenPreview::previewSize(int monitor_number) -> Size {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(monitor_number);

  float ratio = 4 / 3;

  if (!display_info.side().error()) {
    const proto::Rectangle &dimensions = display_info.dimensions();
    ratio = static_cast<float>(dimensions.width()) / dimensions.height();
  }
  return Size{.width = static_cast<int>(PREVIEW_HEIGHT * ratio),
              .height = PREVIEW_HEIGHT};
}

auto ScreenPreview::thumbnailWidget() -> ScreenText * {
  return thumbnail.get();
}

void ScreenPreview::resetFromSettings(int monitor_number) {
  screen_text->setSize(previewSize(monitor_number).toWx());
  proto::ScreenSide side =
      DisplayConfig::getInstance()->displayDetails(monitor_number).side();
  for (auto team : TeamConfig::getInstance()->singleScreenOrder()) {
    if (ProtoUtil::sideContains(side, team)) {
      proto::ScreenSide effective_side = ProtoUtil::teamSide(team);
      screen_text->setBackground(
          TeamConfig::getInstance()->teamColor(effective_side)[0],
          effective_side);
    }
  }
  screen_text->refresh();
}

void ScreenPreview::sendToPresenter(ScreenText *screen_text) {
  presenter->setAll(*screen_text);
  thumbnail->setAll(*screen_text);
}

void ScreenPreview::sendToPresenter() { sendToPresenter(screen_text.get()); }

void ScreenPreview::blackoutPresenter() {
  presenter->blackout();
  thumbnail->blackout();
}

}  // namespace cszb_scoreboard
