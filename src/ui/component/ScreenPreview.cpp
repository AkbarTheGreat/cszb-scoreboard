/*
ui/ScreenPreview.cpp: This class manages the thumbnail preview of a monitor
in the main window.  In addition, the preview owns the ScreenPresenter which
displays to the actual monitor, dispatching the updated view when necessary.

Copyright 2019-2020 Tracy Beck

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

#include "config/DisplayConfig.h"
#include "config/TeamConfig.h"
#include "ui/UiUtil.h"
#include "ui/component/ScreenText.h"
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

  current_widget = ScreenText::getPreview(control_pane, initial_text, sides,
                                          previewSize(monitor_number));

  thumbnail = new ScreenThumbnail(control_pane, monitor_number, current_widget);
  if (!sides[0].error()) {
    presenter = new ScreenPresenter(monitor_number, current_widget);
  }

  positionWidgets();
}

void ScreenPreview::positionWidgets() {
  wxSizer* sizer = UiUtil::sizer(2, 1);
  sizer->Add(thumbnail->widget(), 1, wxLEFT | wxRIGHT | wxTOP | wxALIGN_CENTER);
  sizer->Add(current_widget, 1, wxALL, BORDER_SIZE);
  control_pane->SetSizerAndFit(sizer);
}

wxSize ScreenPreview::previewSize(int monitor_number) {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(monitor_number);

  float ratio = 4 / 3;

  if (!display_info.side().error()) {
    proto::Rectangle dimensions = display_info.dimensions();
    ratio = (float)dimensions.width() / dimensions.height();
  }
  return wxSize(PREVIEW_HEIGHT * ratio, PREVIEW_HEIGHT);
}

wxPanel* ScreenPreview::controlPane() { return control_pane; }

ScreenText* ScreenPreview::widget() { return current_widget; }

ScreenText* ScreenPreview::thumbnailWidget() { return thumbnail->widget(); }

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

void ScreenPreview::sendToPresenter() {
  presenter->widget()->setAll(*current_widget);
  thumbnail->widget()->setAll(*current_widget);
}

void ScreenPreview::blackoutPresenter() {
  presenter->widget()->blackout();
  thumbnail->widget()->blackout();
}

}  // namespace cszb_scoreboard
