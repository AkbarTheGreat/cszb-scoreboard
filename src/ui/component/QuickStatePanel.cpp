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

QuickStateEntry::QuickStateEntry(wxPanel* parent) {
  screen_text = ScreenText::getPreview(
      parent, "", {ProtoUtil::homeSide(), ProtoUtil::awaySide()},
      wxSize(PREVIEW_WIDTH, PREVIEW_HEIGHT));

  screen_text->setAllText("", 1, Color("Gray"), true, ProtoUtil::homeSide());
  screen_text->setAllText("", 1, Color("Gray"), true, ProtoUtil::awaySide());
}

QuickStatePanel::QuickStatePanel(wxWindow* parent) : wxPanel(parent) {
  for (int i = 0; i < 10; ++i) {
    entries.push_back(new QuickStateEntry(this));
  }
  positionWidgets();
  bindEvents();
}

QuickStatePanel::~QuickStatePanel() {
  for (auto entry : entries) {
    entry->screen()->Destroy();
  }

  entries.clear();
}

void QuickStatePanel::positionWidgets() {
  wxGridBagSizer* sizer = new wxGridBagSizer();

  for (int i = 0; i < entries.size(); i++) {
    UiUtil::addToGridBag(sizer, entries[i]->screen(), i, 0);
  }

  SetSizerAndFit(sizer);
}

void QuickStatePanel::bindEvents() {
  for (auto entry : entries) {
    for (auto side : entry->screen()->sides()) {
      // You have to bind events directly to the ScreenTextSide, as mouse events
      // don't propagate up to parent widgets (even if the child widget doesn't
      // have a handler bound for that event, apparently.)
      side->Bind(wxEVT_RIGHT_UP, &QuickStatePanel::setShortcut, this);
      side->Bind(wxEVT_LEFT_UP, &QuickStatePanel::executeShortcut, this);
    }
  }
}

QuickStateEntry* QuickStatePanel::entryForScreen(ScreenText* screen) {
  for (auto entry : entries) {
    if (entry->screen() == screen) {
      return entry;
    }
  }
  return nullptr;
}

void QuickStatePanel::executeShortcut(wxMouseEvent& event) {
  ScreenTextSide* side = (ScreenTextSide*)event.GetEventObject();
  ScreenText* screen = (ScreenText*)side->GetParent();
  MainView* main = (MainView*)GetParent();
  if (!entryForScreen(screen)->isInitialized()) {
    return;
  }
  main->previewPanel()->setToPresenters(screen);
}

void QuickStatePanel::setShortcut(wxMouseEvent& event) {
  ScreenTextSide* side = (ScreenTextSide*)event.GetEventObject();
  ScreenText* screen = (ScreenText*)side->GetParent();
  MainView* main = (MainView*)GetParent();
  main->controlPanel()->updateScreenTextFromSelected(screen);
  entryForScreen(screen)->initialize();
}

}  // namespace cszb_scoreboard
