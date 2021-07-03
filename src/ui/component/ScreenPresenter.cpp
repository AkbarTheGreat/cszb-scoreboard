/*
ui/ScreenPresenter.cpp: This class manages any content which is being
displayed on an external screen.

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

#include "ui/component/ScreenPresenter.h"

#include <wx/gbsizer.h>

#include "config/CommandArgs.h"
#include "config/DisplayConfig.h"
#include "ui/UiUtil.h"
#include "util/Log.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = 0;

ScreenPresenter::ScreenPresenter(int monitor_number, const ScreenText& preview)
    : Frame("Scoreboard", wxDefaultPosition, wxDefaultSize) {
  this->monitor_number = monitor_number;

  if (DisplayConfig::getInstance()->windowedMode()) {
    show(true);
  } else {
    // Set external monitors to be always on top, fullscreen.
    alwaysOnTop(true);
    fullScreen(true);
  }

#ifdef SCOREBOARD_TESTING
  minimize();
#endif

  proto::DisplayInfo display =
      DisplayConfig::getInstance()->displayDetails(monitor_number);
  wxRect screen = ProtoUtil::wxRct(display.dimensions());

  screen_text = std::make_unique<ScreenText>(childPanel());
  screen_text->setupPresenter(preview, screen.GetSize());
  screen_text->SetSize(screen.GetSize());
  LogDebug(wxT("ScreenPresenter %d: %d,%d %d,%d"), monitor_number, screen.x,
           screen.y, screen.width, screen.height);

  positionWidgets();
  setDimensions(screen);
}

void ScreenPresenter::positionWidgets() {
  auto* sizer = new wxGridBagSizer();
  screen_text->addToSizer(sizer, 0, 0);
  setSizer(sizer);
}
}  // namespace cszb_scoreboard
