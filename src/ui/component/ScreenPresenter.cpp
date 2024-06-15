/*
ui/ScreenPresenter.cpp: This class manages any content which is being
displayed on an external screen.

Copyright 2019-2024 Tracy Beck

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

#include "ui/component/ScreenPresenter.h"

#include "config.pb.h"             // for DisplayInfo, Rectangle
#include "config/DisplayConfig.h"  // for DisplayConfig
#include "config/Position.h"       // for Size, Position
#include "ui/widget/Widget.h"      // for NO_BORDER
#include "util/Log.h"              // for LogDebug

namespace cszb_scoreboard {

const int BORDER_SIZE = 0;

ScreenPresenter::ScreenPresenter(int monitor_number, const ScreenText &preview,
                                 Singleton *singleton)
    : Frame("Scoreboard") {
  this->monitor_number = monitor_number;

  if (singleton->displayConfig()->windowedMode()) {
    show(true);
  } else {
    // Set external monitors to be always on top, fullscreen.
    alwaysOnTop(true);
    fullScreen(true);
    show(true);
  }

#ifdef SCOREBOARD_TESTING
  minimize();
#endif

  proto::DisplayInfo display =
      singleton->displayConfig()->displayDetails(monitor_number);
  Size screen_size{.width = display.dimensions().width(),
                   .height = display.dimensions().height()};
  Position screen_pos{.x = display.dimensions().x(),
                      .y = display.dimensions().y()};

  screen_text = std::make_unique<ScreenText>(childPanel());
  screen_text->setSize(screen_size.toWx());
  LogDebug("ScreenPresenter %d: %d,%d", (int)monitor_number,
           (int)screen_size.width, (int)screen_size.height);

  addWidget(*screen_text, 0, 0, NO_BORDER);
  screen_text->setupPresenter(preview, screen_size);
  runSizer();
  setDimensions(screen_pos, screen_size);
}

}  // namespace cszb_scoreboard
