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
#include "ui/component/ScreenText.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = 10;
const char* WELCOME_MESSAGE = "Hello";
const char* ERROR_MESSAGE = "NO\nSCREENS\nFOUND!";
const int PREVIEW_HEIGHT = 320;

ScreenPreview::ScreenPreview(wxWindow* parent, proto::ScreenSide side,
                             int monitor_number) {
  this->parent = parent;

  wxString initial_text;
  if (side.error()) {
    initial_text = ERROR_MESSAGE;
  } else {
    initial_text = WELCOME_MESSAGE;
  }

  control_pane = new wxPanel(parent);

  current_widget = ScreenText::getPreview(control_pane, initial_text, side,
                                          previewSize(monitor_number));

  thumbnail = new ScreenThumbnail(control_pane, monitor_number, current_widget);
  if (!side.error()) {
    presenter = new ScreenPresenter(monitor_number, current_widget);
  }

  positionWidgets();
}

void ScreenPreview::positionWidgets() {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(2, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
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
  current_widget->setBackground(TeamConfig::getInstance()->teamColor(side)[0],
                                side);
  current_widget->Refresh();
}

void ScreenPreview::sendToPresenter(proto::ScreenSide side) {
  if (current_widget->isSide(side)) {
    presenter->widget()->setAll(*current_widget);
    thumbnail->widget()->setAll(*current_widget);
  }
}

void ScreenPreview::blackoutPresenter(proto::ScreenSide side) {
  if (current_widget->isSide(side)) {
    presenter->widget()->blackout();
    thumbnail->widget()->blackout();
  }
}

}  // namespace cszb_scoreboard
