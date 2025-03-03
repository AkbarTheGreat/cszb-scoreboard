/*
ui/component/control/slideshow/SlidePreview.cpp: The UI for a given slide in a
slideshow, including buttons to modify or move the slide around.

Copyright 2024-2025 Tracy Beck

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

#include "ui/component/control/slideshow/SlidePreview.h"

#include <wx/event.h>  // for wxCommandEvent (ptr...

#include "ui/component/control/SlideshowSetup.h"  // for SlideshowSetup
#include "ui/widget/Widget.h"                     // for NO_BORDER

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

constexpr int SPACER_WIDTH = 48;

SlidePreview::SlidePreview(swx::Panel *wx, SlideshowSetup *owning_control,
                           int32_t index)
    : Panel(wx) {
  parent = owning_control;
  this->index = index;
  slide_name = this->label("");
  left_button = this->button("<", true);
  left_button->toolTip("Move left");
  right_button = this->button(">", true);
  right_button->toolTip("Move right");
  remove_button = this->button("X", true);
  remove_button->toolTip("Delete");
  slide_preview = std::make_unique<ImagePreview>(
      childPanel(ImagePreview::PREVIEW_WIDTH, ImagePreview::PREVIEW_HEIGHT));
  slide_name = this->label("No slide");

  bindEvents();
  positionWidgets();
}

// NOTE TO SELF -- left button isn't working yet -- not sure why.
void SlidePreview::bindEvents() {
  left_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { parent->moveSlideLeft(index); });
  right_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { parent->moveSlideRight(index); });
  remove_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { parent->removeSlide(index); });
}

void SlidePreview::positionWidgets() {
  this->addWidgetWithSpan(*slide_name, 0, 0, 1, 5);
  this->addWidgetWithSpan(*slide_preview, 1, 0, 1, 5);
  this->addSpacer({.width = SPACER_WIDTH}, 2, 0, NO_BORDER);
  this->addWidget(*left_button, 2, 1);
  this->addWidget(*remove_button, 2, 2);
  this->addWidget(*right_button, 2, 3);
  this->addSpacer({.width = SPACER_WIDTH}, 2, 4, NO_BORDER);
  this->runSizer();
}

void SlidePreview::clear() {
  slide_name->set("");
  slide_preview->clearImage();
  refresh();
}

void SlidePreview::setName(std::string name) {
  slide_name->set(name);
  refresh();
}

void SlidePreview::setImage(const FilesystemPath &file) {
  slide_preview->setImage(file);
  refresh();
}

}  // namespace cszb_scoreboard
