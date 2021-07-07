/*
ui/widget/ScrollingPanel.cpp: A panel which may scroll around a larger area than
the panel itself.

Copyright 2021 Tracy Beck

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

#include "ui/widget/ScrollingPanel.h"

namespace cszb_scoreboard {

// For now, these are constant for all scrolling windows.  They may need to be
// parameters one day.
const int SCROLL_X_STEP = 0;
const int SCROLL_Y_STEP = 20;

ScrollingPanel::ScrollingPanel(swx::ScrolledWindow *panel) : Panel(nullptr) {
  wx = panel;
}

void ScrollingPanel::runSizer() {
  static bool firstRun = true;
  wx->SetSizer(sizer());
  wx->FitInside();
  wx->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
  if (firstRun) {
    firstRun = false;
  } else {
    // Setting the scroll rate on the initial sizing breaks the scrollbars, but
    // it's necessary on subsequent runs.
    wx->SetScrollRate(SCROLL_X_STEP, SCROLL_Y_STEP);
  }
}

}  // namespace cszb_scoreboard
