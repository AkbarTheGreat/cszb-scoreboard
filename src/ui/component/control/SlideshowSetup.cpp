/*
ui/component/control/SlideshowSetup.cpp: This class is responsible for managing
a slideshow.  Typically, these are used for pre-show distractions.

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

#include "ui/component/control/SlideshowSetup.h"

#include "config/SlideShow.h"
#include "slide_show.pb.h"

namespace cszb_scoreboard {
class ScreenText;

constexpr int NUM_PREVIEWS = 5;

auto SlideshowSetup ::Create(swx::Panel *wx)
    -> std::unique_ptr<SlideshowSetup> {
  auto control = std::make_unique<SlideshowSetup>(wx);
  control->initializeWidgets();
  return control;
}

void SlideshowSetup::createControls(Panel *control_panel) {
  main_panel = control_panel->panel();
  add_button = main_panel->button("Add New Slide");
  slide_panel = main_panel->panel();

  slide_previews.reserve(NUM_PREVIEWS);
  for (int i = 0; i < NUM_PREVIEWS; i++) {
    slide_previews.emplace_back(
        std::make_unique<SlidePreview>(slide_panel->childPanel(
            SlidePreview::PREVIEW_WIDTH, SlidePreview::PREVIEW_HEIGHT)));
  }

  positionWidgets(control_panel);
  bindEvents();
  setSlidePreviews(0);
}

void SlideshowSetup::positionWidgets(Panel *control_panel) {
  control_panel->addWidget(*main_panel, 0, 0);

  main_panel->addWidget(*add_button, 0, 0);
  main_panel->addWidgetWithSpan(*slide_panel, 1, 0, 1, 2);
  for (int i = 0; i < NUM_PREVIEWS; i++) {
    slide_panel->addWidget(*slide_previews[i], 0, i);
  }

  slide_panel->runSizer();
  main_panel->runSizer();
  control_panel->runSizer();
}

void SlideshowSetup::bindEvents() {
  add_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->addNewSlide(); });
}

void SlideshowSetup::updateScreenText(ScreenText *screen_text) {}

void SlideshowSetup::addNewSlide() {}

void SlideshowSetup::setSlidePreviews(unsigned int page_number) {
  current_slide_page = page_number;

  int32_t start = page_number * NUM_PREVIEWS;
  int32_t end = start + NUM_PREVIEWS;
  std::vector<proto::SlideInfo> slides =
      singleton->slideShow()->slides(start, end);

  if (slides.empty() && page_number > 0) {
    setSlidePreviews(page_number - 1);
    return;
  }

  for (int i = 0; i < slides.size(); i++) {
    slide_previews[i]->setImage(FilesystemPath(slides[i].file_path()));
    slide_previews[i]->setName(slides[i].name());
  }

  for (int i = slides.size(); i < NUM_PREVIEWS; i++) {
    slide_previews[i]->clear();
  }
}

}  // namespace cszb_scoreboard
