/*
ui/ScreenPresenter.cpp: This class manages any content which is being
displayed on an external screen.

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

#include "ui/component/ScreenPresenter.h"

#include "config/DisplayConfig.h"
#include "ui/frame/FrameList.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {
ScreenPresenter::ScreenPresenter(int monitor_number, ScreenText* widget)
    : wxFrame(NULL, wxID_ANY, "Scoreboard", wxDefaultPosition, wxDefaultSize) {
  this->monitor_number = monitor_number;
#ifndef WXDEBUG
  // Set external monitors to be always on top, unless we're debugging, since
  // sometimes we use one monitor to debug
  SetWindowStyle(GetWindowStyle() | wxSTAY_ON_TOP);
#endif
  ShowFullScreen(true);

  proto::DisplayInfo display =
      DisplayConfig::getInstance()->displayDetails(monitor_number);
  wxRect screen = ProtoUtil::wxRct(display.dimensions());

  screen_text = ScreenText::getPresenter(this, widget, screen.GetSize());
  screen_text->SetSize(screen.GetSize());
  wxLogDebug(wxT("ScreenPresenter %d: %d,%d %d,%d"), monitor_number, screen.x,
             screen.y, screen.width, screen.height);
  FrameList::getInstance()->addFrame(this);

  positionWidgets();
  SetPosition(screen.GetPosition());
  SetSize(screen.GetSize());
}

void ScreenPresenter::positionWidgets() {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 2, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  sizer->Add(screen_text, 1, wxEXPAND | wxALL, 0);
  SetSizerAndFit(sizer);
}
}  // namespace cszb_scoreboard
