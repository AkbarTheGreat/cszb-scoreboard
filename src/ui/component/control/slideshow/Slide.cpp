/*
ui/component/control/slideshow/Slide.cpp: The UI for a given slide in a
slideshow, including buttons to modify or move the slide around.

Copyright 2024 Tracy Beck

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

#include "ui/component/control/slideshow/Slide.h"

#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

Slide::Slide(swx::Panel *wx, FilesystemPath image) : Panel(wx) {
  slide_name = this->label(image.filename().string());
  left_button = this->button("<");
  right_button = this->button(">");
  remove_button = this->button("X");
  slide_preview = std::make_unique<ImagePreview>(
      childPanel(ImagePreview::PREVIEW_WIDTH, ImagePreview::PREVIEW_HEIGHT));
  slide_preview->setImage(image);

  bindEvents();
  positionWidgets();
}

void Slide::bindEvents() {}

void Slide::positionWidgets() {
  this->addWidgetWithSpan(*slide_name, 0, 0, 1, 3);
  this->addWidgetWithSpan(*slide_preview, 1, 0, 1, 3);
  this->addWidget(*left_button, 2, 0);
  this->addWidget(*remove_button, 2, 1);
  this->addWidget(*right_button, 2, 2);
  this->runSizer();
}

}  // namespace cszb_scoreboard
