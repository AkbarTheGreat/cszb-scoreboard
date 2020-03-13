/*
ui/component/PreviewPanel.cpp: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

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
#include "ui/component/PreviewPanel.h"

#include "config/DisplayConfig.h"
#include "config/TeamConfig.h"
#include "ui/component/Menu.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

PreviewPanel::PreviewPanel(wxWindow* parent) : wxPanel(parent) {
  aui_manager.SetManagedWindow(this);
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    if (display_info.side().error() || display_info.side().home() ||
        display_info.side().away()) {
      screens.push_back(new ScreenPreview(this, display_info.side(), display_info.id()));
    }
  }

  positionWidgets();
  bindEvents();
}

PreviewPanel::~PreviewPanel() { aui_manager.UnInit(); }

void PreviewPanel::positionWidgets() {
  wxAuiPaneInfo pane_style;
  pane_style.CenterPane();
  pane_style.Top();
  pane_style.CloseButton(false);
  for (auto screen : screens) {
    wxPanel* pane = screen->controlPane();
    pane_style.MinSize(pane->GetSize());
    // TODO: write something like pane_style.Position(screen.position()) to
    // allow users to change the order of monitors.
    aui_manager.AddPane(pane, pane_style);
  }
  aui_manager.Update();
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

void PreviewPanel::setImageForPreview(const wxImage& image,
                                      const proto::ScreenSide& side) {
  for (auto preview : screens) {
    ScreenText* screen_text = preview->widget();
    screen_text->setImage(image, true, side);
    screen_text->Refresh();
  }
}

void PreviewPanel::setTextForPreview(wxString text, int font_size,
                                     const Color& background, bool auto_fit,
                                     const proto::ScreenSide& side) {
  for (auto preview : screens) {
    ScreenText* screen_text = preview->widget();
    screen_text->setAutoFit(auto_fit, side);
    screen_text->setBackground(background, side);
    screen_text->setText(text, font_size, side);
    screen_text->Refresh();
  }
}

void PreviewPanel::setTextForPreview(std::vector<proto::RenderableText> lines,
                                     const Color& background, bool auto_fit,
                                     const proto::ScreenSide& side) {
  for (auto preview : screens) {
    ScreenText* screen_text = preview->widget();
    screen_text->setAutoFit(auto_fit, side);
    screen_text->resetAllText(side);
    screen_text->setBackground(background, side);
    for (auto line : lines) {
      ProtoUtil::validateFont(line.mutable_font());
      screen_text->setFontColor(line.mutable_font());
      screen_text->addText(line, side);
      screen_text->Refresh();
    }
  }
}

void PreviewPanel::updatePresenters(const proto::ScreenSide& side) {
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
