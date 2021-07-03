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

#include <wx/gbsizer.h>

#include "config/DisplayConfig.h"
#include "config/TeamConfig.h"
#include "ui/UiUtil.h"
#include "ui/component/ScreenText.h"
#include "ui/frame/FrameManager.h"
#include "ui/widget/swx/Panel.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = 10;
const char* WELCOME_MESSAGE = "Hello";
const char* ERROR_MESSAGE = "NO\nSCREENS\nFOUND!";
const int PREVIEW_HEIGHT = 320;

ScreenPreview::ScreenPreview(swx::Panel* wx,
                             std::vector<proto::ScreenSide> sides,
                             int monitor_number)
    : Panel(wx) {
  this->parent = parent;

  wxString initial_text;
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
  auto* sizer = new wxGridBagSizer();
  thumbnail->addToSizer(sizer, 0, 0, 1, 1, DEFAULT_BORDER_SIZE,
                        wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER);
  screen_text->addToSizer(sizer, 1, 0, 1, 1, BORDER_SIZE, wxALL);
  SetSizerAndFit(sizer);
}

auto ScreenPreview::previewSize(int monitor_number) -> wxSize {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(monitor_number);

  float ratio = 4 / 3;

  if (!display_info.side().error()) {
    const proto::Rectangle& dimensions = display_info.dimensions();
    ratio = static_cast<float>(dimensions.width()) / dimensions.height();
  }
  return wxSize(PREVIEW_HEIGHT * ratio, PREVIEW_HEIGHT);
}

auto ScreenPreview::controlPane() -> wxPanel* { return wx; }

auto ScreenPreview::thumbnailWidget() -> ScreenText* { return thumbnail.get(); }

void ScreenPreview::resetFromSettings(int monitor_number) {
  screen_text->SetSize(previewSize(monitor_number));
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
  screen_text->Refresh();
}

void ScreenPreview::sendToPresenter(ScreenText* screen_text) {
  presenter->setAll(*screen_text);
  thumbnail->setAll(*screen_text);
}

void ScreenPreview::sendToPresenter() { sendToPresenter(screen_text.get()); }

void ScreenPreview::blackoutPresenter() {
  presenter->blackout();
  thumbnail->blackout();
}

}  // namespace cszb_scoreboard
