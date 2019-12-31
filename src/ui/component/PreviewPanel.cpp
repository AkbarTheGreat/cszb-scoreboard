/*
ui/component/PreviewPanel.cpp: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

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
#include "ui/component/PreviewPanel.h"

#include "config/DisplayConfig.h"
#include "ui/component/Menu.h"

namespace cszb_scoreboard {

const int BORDER_WIDTH = 10;

PreviewPanel::PreviewPanel(wxWindow* parent) : wxPanel(parent) {
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    if (display_info.side().error() || display_info.side().home() ||
        display_info.side().away()) {
      screens.push_back(new ScreenPreview(this, display_info.side(), i));
    }
  }

  positionWidgets();
  bindEvents();
}

void PreviewPanel::positionWidgets() {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(1, 0, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto screen : screens) {
    sizer->Add(screen->widget(), 1, wxEXPAND | wxALL, BORDER_WIDTH);
  }
  SetSizerAndFit(sizer);
}

void PreviewPanel::bindEvents() {
  GetParent()->Bind(wxEVT_COMMAND_MENU_SELECTED, &PreviewPanel::blackout, this,
                    DISPLAY_BLACK_OUT);
}

int PreviewPanel::numPreviews() { return screens.size(); }

ScreenPreview* PreviewPanel::preview(int index) {
  assert(index >= 0 && index < screens.size());
  return screens[index];
}

void PreviewPanel::setTextForPreview(wxString text, int font_size,
                                     proto::ScreenSide side) {
  for (auto preview : screens) {
    ScreenText* screen_text = preview->widget();
    screen_text->setText(text, font_size, side);
    screen_text->Refresh();
  }
}

void PreviewPanel::updatePresenters(proto::ScreenSide side) {
  for (auto preview : screens) {
    preview->sendToPresenter(side);
  }
}

void PreviewPanel::updatePreviewsFromSettings() {
  // Eventually, we may have the possibility for settings to reset where we now
  // have more/less screens than we did before.  When that happens, this method
  // needs to deal with that case, too.

  int screen_index = 0;
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    if (display_info.side().error() || display_info.side().home() ||
        display_info.side().away()) {
      screens[screen_index++]->resetFromSettings(i);
    }
  }
}

void PreviewPanel::blackout(wxCommandEvent& event) {
  proto::ScreenSide side;
  // Always blackout all screens
  side.set_home(true);
  side.set_away(true);
  side.set_extra(true);
  for (auto preview : screens) {
    preview->blackoutPresenter(side);
  }
}

}  // namespace cszb_scoreboard
