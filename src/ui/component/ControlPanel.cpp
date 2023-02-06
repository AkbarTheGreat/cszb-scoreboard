/*
ui/component/ControlPanel.cpp: Holds the controls for changing the contents on a
screen.

Copyright 2019-2023 Tracy Beck

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

#include <wx/aui/auibook.h>  // for wxAuiNotebookEvent

#include <utility>  // for move

#include "ui/component/control/ImageFromLibrary.h"  // for ImageFromLibrary
#include "ui/component/control/LocalImage.h"        // for LocalImage
#include "ui/component/control/ScoreControl.h"      // for ScoreControl
#include "ui/component/control/TextEntry.h"         // for TextEntry
#include "ui/component/control/ThingsMode.h"        // for ThingsMode
#include "ui/component/control/TimerSetup.h"

namespace cszb_scoreboard {
class ScreenText;

namespace swx {
class Notebook;
}  // namespace swx

ControlPanel::ControlPanel(swx::Notebook *wx) : Notebook(wx) {
  addController(std::move(ScoreControl::Create(childPanel())), "Score");
  addController(std::move(ImageFromLibrary::Create(childPanel())),
                "Image Library");
  addController(std::move(LocalImage::Create(childPanel())), "Load Image");
  addController(std::move(ThingsMode::Create(childPanel())), "5/6 Things");
  addController(std::move(TextEntry::Create(childPanel())), "Text");
  addController(std::move(TimerSetup::Create(childPanel())), "Timer");

  bindEvents();

  // Force proper initialization of the preview at application start.
  controllers[0]->updatePreview();
}

void ControlPanel::addController(std::unique_ptr<ScreenTextController> tab,
                                 const std::string &name) {
  addTab(*tab, name);
  controllers.push_back(std::move(tab));
}

void ControlPanel::bindEvents() {
  bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED,
       [this](wxAuiNotebookEvent &event) -> void { this->tabChanged(event); });
}

void ControlPanel::tabChanged(const wxAuiNotebookEvent &event) {
  controllers[event.GetSelection()]->updatePreview();
}

void ControlPanel::updateScreenTextFromSelected(ScreenText *screen_text) {
  controllers[selection()]->updateScreenText(screen_text);
}

auto ControlPanel::isSelected(ScreenTextController *controller) -> bool {
  return controllers[selection()].get() == controller;
}

}  // namespace cszb_scoreboard
