/*
ui/component/control/ScreenTextController.cpp: Any class which extends this
serves to set information from the user into a ScreenText.  It could be
arbitrary text, a single image, or a score update, to name a few.

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

#include "ui/component/control/ScreenTextController.h"

#include "ui/UiUtil.h"
#include "ui/frame/HotkeyTable.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ScreenTextController::ScreenTextController(PreviewPanel *preview_panel,
                                           wxWindow *parent)
    : wxPanel(parent) {
  this->preview_panel = preview_panel;
}

void ScreenTextController::initializeWidgets() {
  update_screens = new wxButton(this, wxID_ANY, "Send to Monitors");
  update_screens->SetToolTip("Ctrl+Space");
  HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, WXK_SPACE,
                                        update_screens->GetId());
  control_panel = new wxPanel(this);
  createControls(control_panel);
  positionWidgets();
  bindEvents();
}

void ScreenTextController::positionWidgets() {
  wxSizer *sizer = UiUtil::sizer(0, 1);

  sizer->Add(control_panel, 0, wxALL, BORDER_SIZE);
  sizer->Add(update_screens, 0, wxALL, BORDER_SIZE);

  SetSizerAndFit(sizer);
}

void ScreenTextController::bindEvents() {
  update_screens->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                       &ScreenTextController::updateClicked, this);
}

PreviewPanel *ScreenTextController::previewPanel() { return preview_panel; }

void ScreenTextController::updateClicked(wxCommandEvent &event) {
  preview_panel->updatePresenters();
}

}  // namespace cszb_scoreboard
