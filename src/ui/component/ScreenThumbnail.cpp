/*
ui/component/ScreenThumbnail.cpp: Represents a very small view of exactly what's
on the matching ScreenPresenter, in case there's not a great view from the booth
to all monitors.

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

#include "ui/component/ScreenThumbnail.h"

#include "config/DisplayConfig.h"

namespace cszb_scoreboard {

const int THUMBNAIL_HEIGHT = 64;

ScreenThumbnail::ScreenThumbnail(wxWindow* parent, int monitor_number,
                                 ScreenText* widget) {
  screen_text =
      ScreenText::getPresenter(parent, widget, thumbnailSize(monitor_number));
}

auto ScreenThumbnail::thumbnailSize(int monitor_number) -> wxSize {
  proto::DisplayInfo display_info =
      DisplayConfig::getInstance()->displayDetails(monitor_number);

  float ratio = 4 / 3;

  if (!display_info.side().error()) {
    const proto::Rectangle& dimensions = display_info.dimensions();
    ratio = static_cast<float>(dimensions.width()) / dimensions.height();
  }
  return wxSize(THUMBNAIL_HEIGHT * ratio, THUMBNAIL_HEIGHT);
}

}  // namespace cszb_scoreboard
