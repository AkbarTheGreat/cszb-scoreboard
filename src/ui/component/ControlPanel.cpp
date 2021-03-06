/*
ui/component/ControlPanel.cpp: Holds the controls for changing the contents on a
screen.

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

#include "ui/component/ControlPanel.h"

#include "ui/component/control/ImageFromLibrary.h"
#include "ui/component/control/LocalImage.h"
#include "ui/component/control/ScoreControl.h"
#include "ui/component/control/TextEntry.h"
#include "ui/component/control/ThingsMode.h"

namespace cszb_scoreboard {

const int NOTEBOOK_STYLE = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT |
                           wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;

ControlPanel::ControlPanel(wxWindow* parent, PreviewPanel* preview_panel)
    : wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                    NOTEBOOK_STYLE) {
  controllers.push_back(ScoreControl::Create(preview_panel, this));
  AddPage(controllers.back(), "Score");

  controllers.push_back(ImageFromLibrary::Create(preview_panel, this));
  AddPage(controllers.back(), "Image Library");

  controllers.push_back(LocalImage::Create(preview_panel, this));
  AddPage(controllers.back(), "Load Image");

  controllers.push_back(ThingsMode::Create(preview_panel, this));
  AddPage(controllers.back(), "5/6 Things");

  controllers.push_back(TextEntry::Create(preview_panel, this));
  AddPage(controllers.back(), "Text");

  bindEvents();

  // Force proper initialization of the preview at application start.
  controllers[0]->updatePreview();
}

void ControlPanel::bindEvents() {
  Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &ControlPanel::tabChanged, this);
}

void ControlPanel::tabChanged(wxAuiNotebookEvent& event) {
  controllers[event.GetSelection()]->updatePreview();
}

void ControlPanel::updateScreenTextFromSelected(ScreenText* screen_text) {
  controllers[GetSelection()]->updateScreenText(screen_text);
}

}  // namespace cszb_scoreboard
