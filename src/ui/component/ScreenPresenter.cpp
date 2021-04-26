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

#include "config/CommandArgs.h"
#include "config/DisplayConfig.h"
#include "ui/UiUtil.h"
#include "ui/frame/FrameList.h"
#include "util/Log.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = 0;

ScreenPresenter::ScreenPresenter(int monitor_number, ScreenText* widget)
    : wxFrame(nullptr, wxID_ANY, "Scoreboard", wxDefaultPosition,
              wxDefaultSize) {
  this->monitor_number = monitor_number;

  if (DisplayConfig::getInstance()->windowedMode()) {
    Show(true);
  } else {
    // Set external monitors to be always on top.
    SetWindowStyle(GetWindowStyle() | wxSTAY_ON_TOP);
    ShowFullScreen(true);
  }

#ifdef SCOREBOARD_TESTING
  Iconize();
#endif

  proto::DisplayInfo display =
      DisplayConfig::getInstance()->displayDetails(monitor_number);
  wxRect screen = ProtoUtil::wxRct(display.dimensions());

  screen_text = ScreenText::getPresenter(this, widget, screen.GetSize());
  screen_text->SetSize(screen.GetSize());
  LogDebug(wxT("ScreenPresenter %d: %d,%d %d,%d"), monitor_number, screen.x,
           screen.y, screen.width, screen.height);
  FrameList::getInstance()->addFrame(this);

  positionWidgets();
  SetPosition(screen.GetPosition());
  SetSize(screen.GetSize());
}

void ScreenPresenter::positionWidgets() {
  wxSizer* sizer = UiUtil::sizer(0, 2);
  sizer->Add(screen_text, 1, wxEXPAND | wxALL, BORDER_SIZE);
  SetSizerAndFit(sizer);
}
}  // namespace cszb_scoreboard
