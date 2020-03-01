/*
ui/component/ControlPanel.cpp: Holds the controls for changing the contents on a
screen.

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

#include "ui/component/ControlPanel.h"

#include "ui/component/control/LocalImage.h"
#include "ui/component/control/ScoreControl.h"
#include "ui/component/control/TextEntry.h"
#include "ui/component/control/ThingsMode.h"

namespace cszb_scoreboard {

ControlPanel::ControlPanel(wxWindow* parent, PreviewPanel* preview_panel)
    : wxNotebook(parent, wxID_ANY) {
  // TODO: Put this tab at the back of the notebook before merging back to trunk
  controllers.push_back(ThingsMode::Create(preview_panel, this));
  AddPage(controllers.back(), "5/6 Things");

  controllers.push_back(TextEntry::Create(preview_panel, this));
  AddPage(controllers.back(), "Text");

  controllers.push_back(ScoreControl::Create(preview_panel, this));
  AddPage(controllers.back(), "Score");

  controllers.push_back(LocalImage::Create(preview_panel, this));
  AddPage(controllers.back(), "Image");

  controllers.push_back(ThingsMode::Create(preview_panel, this));
  AddPage(controllers.back(), "5/6 Things");

  bindEvents();
}

void ControlPanel::bindEvents() {
  Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &ControlPanel::tabChanged, this);
}

void ControlPanel::tabChanged(wxBookCtrlEvent& event) {
  controllers[event.GetSelection()]->updatePreview();
}

}  // namespace cszb_scoreboard
