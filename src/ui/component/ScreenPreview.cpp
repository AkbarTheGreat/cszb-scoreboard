/*
ui/ScreenPreview.cpp: This class manages the thumbnail preview of a monitor
in the main window.  In addition, the preview owns the ScreenPresenter which
displays to the actual monitor, dispatching the updated view when necessary.

Copyright 2019 Tracy Beck

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
#include "ui/component/ScreenText.h"

namespace cszb_scoreboard {

ScreenPreview::ScreenPreview(wxWindow* parent, proto::ScreenSide side,
                             int monitor_number) {
  this->parent = parent;
  ScreenText* screen_text = nullptr;
  float ratio = 4 / 3;
  if (!side.error()) {
    proto::Rectangle dimensions = DisplayConfig::getInstance()
                                      ->displayDetails(monitor_number)
                                      .dimensions();
    ratio = (float)dimensions.width() / dimensions.height();
  }
  screen_text = ScreenText::getPreview(parent, ratio, side);
  if (!side.error()) {
    presenter = new ScreenPresenter(
        monitor_number,
        screen_text);  // TODO: Pass along screen number that's expected
  }
  this->current_widget = screen_text;
}

ScreenText* ScreenPreview::widget() { return current_widget; }

void ScreenPreview::sendToPresenter(proto::ScreenSide side) {
  if (current_widget->isSide(side)) {
    presenter->widget()->setAll(*current_widget);
  }
}

void ScreenPreview::blackoutPresenter(proto::ScreenSide side) {
  if (current_widget->isSide(side)) {
    presenter->widget()->blackout();
  }
}

}  // namespace cszb_scoreboard
