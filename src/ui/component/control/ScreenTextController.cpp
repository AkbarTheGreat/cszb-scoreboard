/*
ui/component/control/ScreenTextController.cpp: Any class which extends this
serves to set information from the user into a ScreenText.  It could be
arbitrary text, a single image, or a score update, to name a few.

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

#include "ui/component/control/ScreenTextController.h"

#include <wx/accel.h>
#include <wx/defs.h>
#include <wx/event.h>

#include "ScoreboardCommon.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/ScreenPreview.h"
#include "ui/frame/HotkeyTable.h"
#include "ui/widget/Widget.h"

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ScreenTextController::ScreenTextController(PreviewPanel *preview_panel,
                                           swx::Panel *wx)
    : Panel(wx) {
  this->preview_panel = preview_panel;
}

void ScreenTextController::initializeWidgets() {
  update_screens = button("Send to Monitors");
  update_screens->toolTip("Ctrl+Space");
  HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, WXK_SPACE,
                                        update_screens->id());
  control_panel = std::make_unique<Panel>(childPanel());
  createControls(control_panel.get());
  positionWidgets();
  bindEvents();
}

void ScreenTextController::positionWidgets() {
  // control_panel spans two columns so that update_screens isn't forced to
  // stretch the the width of whatever the control_panel is.
  addWidgetWithSpan(*control_panel, 0, 0, 1, 2, NO_BORDER);
  addWidget(*update_screens, 1, 0);

  runSizer();
}

void ScreenTextController::bindEvents() {
  update_screens->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->updateClicked(); });
}

auto ScreenTextController::previewPanel() -> PreviewPanel * {
  return preview_panel;
}

void ScreenTextController::updateClicked() {
  preview_panel->updatePresenters();
}

void ScreenTextController::updatePreview() {
  previewPanel()->forAllScreens([this](ScreenPreview *preview) -> void {
    this->updateScreenText(preview->screen());
  });
}

}  // namespace cszb_scoreboard
