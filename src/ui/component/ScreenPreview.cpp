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

ScreenPreview::ScreenPreview(wxWindow* parent,
                             std::vector<proto::ScreenSide> sides,
                             int monitor_number) {
  this->parent = parent;

  wxString initial_text;
  if (sides[0].error()) {
    initial_text = ERROR_MESSAGE;
  } else {
    initial_text = WELCOME_MESSAGE;
  }

  control_pane = new wxPanel(parent);

  current_widget = new ScreenText(new swx::Panel(control_pane));
  current_widget->setupPreview(initial_text, sides,
                               previewSize(monitor_number));

  thumbnail = new ScreenThumbnail(control_pane, monitor_number, current_widget);
  if (!sides[0].error()) {
    presenter = FrameManager::getInstance()->createScreenPresenter(
        monitor_number, current_widget);
  }

  positionWidgets();
}

void ScreenPreview::positionWidgets() {
  auto* sizer = new wxGridBagSizer();
  thumbnail->widget()->addToSizer(sizer, 0, 0, 1, 1, DEFAULT_BORDER_SIZE,
                                  wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER);
  current_widget->addToSizer(sizer, 1, 0, 1, 1, BORDER_SIZE, wxALL);
  control_pane->SetSizerAndFit(sizer);
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

auto ScreenPreview::controlPane() -> wxPanel* { return control_pane; }

auto ScreenPreview::widget() -> ScreenText* { return current_widget; }

auto ScreenPreview::thumbnailWidget() -> ScreenText* {
  return thumbnail->widget();
}

void ScreenPreview::resetFromSettings(int monitor_number) {
  current_widget->SetSize(previewSize(monitor_number));
  proto::ScreenSide side =
      DisplayConfig::getInstance()->displayDetails(monitor_number).side();
  for (auto team : TeamConfig::getInstance()->singleScreenOrder()) {
    if (ProtoUtil::sideContains(side, team)) {
      proto::ScreenSide effective_side = ProtoUtil::teamSide(team);
      current_widget->setBackground(
          TeamConfig::getInstance()->teamColor(effective_side)[0],
          effective_side);
    }
  }
  current_widget->Refresh();
}

void ScreenPreview::sendToPresenter(ScreenText* screen_text) {
  presenter->setAll(*screen_text);
  thumbnail->widget()->setAll(*screen_text);
}

void ScreenPreview::sendToPresenter() { sendToPresenter(current_widget); }

void ScreenPreview::blackoutPresenter() {
  presenter->blackout();
  thumbnail->widget()->blackout();
}

}  // namespace cszb_scoreboard
