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
#include "ui/widget/FilePicker.h"

namespace cszb_scoreboard {
class ScreenText;

constexpr int NUM_PREVIEWS = 5;

auto SlideshowSetup::Create(swx::Panel *wx) -> std::unique_ptr<SlideshowSetup> {
  auto control = std::make_unique<SlideshowSetup>(wx);
  control->initializeWidgets();
  return control;
}

void SlideshowSetup::swapSlides(int32_t a, int32_t b) {
  singleton->slideShow()->swapSlides(a, b);
  singleton->slideShow()->saveShow();
  setSlidePreviews(current_slide_page);
}

void SlideshowSetup::moveSlideLeft(int32_t index) {
  int32_t a = index + (NUM_PREVIEWS * current_slide_page);
  swapSlides(a, a - 1);
}

void SlideshowSetup::moveSlideRight(int32_t index) {
  int32_t a = index + (NUM_PREVIEWS * current_slide_page);
  swapSlides(a, a + 1);
}

void SlideshowSetup::removeSlide(int32_t index) {
  index = index + (NUM_PREVIEWS * current_slide_page);
  singleton->slideShow()->removeSlide(index);
  singleton->slideShow()->saveShow();
  setSlidePreviews(current_slide_page);
}

void SlideshowSetup::createControls(Panel *control_panel) {
  main_panel = control_panel->panel();
  slide_panel = main_panel->panel();

  add_button = main_panel->button("Add New Slide");
  previous_page = main_panel->button("<");
  previous_page->toolTip("Previous Page");
  next_page = main_panel->button(">");
  next_page->toolTip("Next Page");

  slide_previews.reserve(NUM_PREVIEWS);
  for (int i = 0; i < NUM_PREVIEWS; i++) {
    slide_previews.emplace_back(std::make_unique<SlidePreview>(
        slide_panel->childPanel(SlidePreview::PREVIEW_WIDTH,
                                SlidePreview::PREVIEW_HEIGHT),
        this, i));
  }

  positionWidgets(control_panel);
  bindEvents();
  setSlidePreviews(0);
}

void SlideshowSetup::positionWidgets(Panel *control_panel) {
  control_panel->addWidget(*main_panel, 0, 0);

  main_panel->addWidgetWithSpan(*add_button, 0, 0, 1, 2);
  main_panel->addWidget(*previous_page, 1, 0);
  main_panel->addWidget(*next_page, 1, 1);
  main_panel->addWidgetWithSpan(*slide_panel, 2, 0, 1, 3);
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
  previous_page->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->previousPage(); });
  next_page->bind(wxEVT_COMMAND_BUTTON_CLICKED,
                  [this](wxCommandEvent &event) -> void { this->nextPage(); });
}

void SlideshowSetup::updateScreenText(ScreenText *screen_text) {}

void SlideshowSetup::addNewSlide() {
  std::unique_ptr<FilePicker> dialog =
      openFilePicker("Select Image", IMAGE_SELECTION_STRING);
  std::optional<FilesystemPath> new_file = dialog->selectFile();
  if (new_file.has_value()) {
    std::string name = new_file->filename().string();
    singleton->slideShow()->addSlide(name, *new_file);
    singleton->slideShow()->saveShow();
    setSlidePreviews(current_slide_page);
  }
  lastPage();
}

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

void SlideshowSetup::previousPage() {
  if (current_slide_page <= 0) return;
  setSlidePreviews(current_slide_page - 1);
}

void SlideshowSetup::nextPage() { setSlidePreviews(current_slide_page + 1); }

void SlideshowSetup::lastPage() {
  int previous_page = -1;
  while (previous_page != current_slide_page) {
    previous_page = current_slide_page;
    nextPage();
  }
}

}  // namespace cszb_scoreboard
