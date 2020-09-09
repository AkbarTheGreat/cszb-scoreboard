/*
ui/component/QuickStatePanel.cpp: This panel provides quick-settable screens.

Copyright 2020 Tracy Beck

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

#include "ui/component/QuickStatePanel.h"

#include "ui/UiUtil.h"
#include "ui/frame/MainView.h"
#include "util/ProtoUtil.h"
#include "wx/gbsizer.h"

namespace cszb_scoreboard {

const int PREVIEW_WIDTH = 85;  // a thumbnail-sized 4x3 display
const int PREVIEW_HEIGHT = 64;

QuickStatePanel::QuickStatePanel(wxWindow* parent) : wxPanel(parent) {
  // std::vector<proto::ScreenSide> = {ProtoUtil::allSide()}
  for (int i = 0; i < 10; ++i) {
    ScreenText* screen_text = ScreenText::getPreview(
        this, "X", {ProtoUtil::homeSide(), ProtoUtil::awaySide()},
        wxSize(PREVIEW_WIDTH, PREVIEW_HEIGHT));
    screen_text->blackout();
    screen_texts.push_back(screen_text);
  }
  positionWidgets();
  bindEvents();
}

QuickStatePanel::~QuickStatePanel() {
  for (auto screen_text : screen_texts) {
    screen_text->Destroy();
  }

  screen_texts.clear();
}

void QuickStatePanel::positionWidgets() {
  wxGridBagSizer* sizer = new wxGridBagSizer();

  for (int i = 0; i < screen_texts.size(); i++) {
    UiUtil::addToGridBag(sizer, screen_texts[i], i, 0);
  }

  SetSizerAndFit(sizer);
}

void QuickStatePanel::bindEvents() {
  for (auto screen : screen_texts) {
    for (auto side : screen->sides()) {
      side->Bind(wxEVT_RIGHT_UP, &QuickStatePanel::setShortcut, this);
      side->Bind(wxEVT_LEFT_UP, &QuickStatePanel::executeShortcut, this);
    }
  }
}

void QuickStatePanel::executeShortcut(wxMouseEvent& event) {
  setShortcut(event);
  event.Skip();
}

void QuickStatePanel::setShortcut(wxMouseEvent& event) {
  ScreenTextSide* side = (ScreenTextSide*)event.GetEventObject();
  ScreenText* screen = (ScreenText*)side->GetParent();
  MainView* main = (MainView*)GetParent();
  main->controlPanel()->updateScreenTextFromSelected(screen);
  event.Skip();
}

}  // namespace cszb_scoreboard
