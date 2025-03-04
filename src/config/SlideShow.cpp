/*
config/SlideShow.cpp: This class is a singleton which
contains information for a slide show.

Copyright 2025 Tracy Beck

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

#include "config/SlideShow.h"

#include <algorithm>  // for max
#include <utility>    // for move

#include "config/Persistence.h"   // for Persistence
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/Singleton.h"       // for Singleton, SingletonClass

namespace cszb_scoreboard {

SlideShow::SlideShow(SingletonClass c, Singleton *singleton) {
  this->singleton = singleton;
  this->slide_show =
      std::move(Singleton::getInstance()->persistence()->loadSlideShow());
}

void SlideShow::swapSlides(int32_t a, int32_t b) {
  if (a < 0 || b < 0) {
    return;
  }
  if (a >= slide_show.slides_size() || b >= slide_show.slides_size()) {
    return;
  }
  slide_show.mutable_slides()->SwapElements(a, b);
}

void SlideShow::removeSlide(int32_t index) {
  for (int i = index; i < slide_show.mutable_slides()->size() - 1; ++i) {
    slide_show.mutable_slides()->SwapElements(i, i + 1);
  }
  slide_show.mutable_slides()->RemoveLast();
}

// -1 index is the default and means to add the slide to the end of the list.
void SlideShow::addSlide(const std::string &name, const FilesystemPath &file,
                         int32_t index) {
  proto::SlideInfo *slide = slide_show.add_slides();
  slide->set_name(name);
  slide->set_file_path(file.string());
  if (index == -1) {
    return;
  }
  int i = slide_show.mutable_slides()->size() - 1;
  while (i > index) {
    swapSlides(i, i - 1);
    --i;
  }
}

void SlideShow::saveShow() {
  singleton->persistence()->saveSlideShow(slide_show);
}

auto SlideShow::slides(int32_t start, int32_t end)
    -> std::vector<proto::SlideInfo> {
  std::vector<proto::SlideInfo> slides;
  for (int32_t index = start; index < end && index < slide_show.slides_size();
       index++) {
    slides.push_back(slide_show.slides(index));
  }
  return slides;
}

void SlideShow::start() {
  is_running = true;
  last_transition = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch());
}
void SlideShow::stop() { is_running = false; }

auto SlideShow::nextSlide() -> Image {
  // We can't expect to run an empty slide show.
  if (slide_show.slides_size() == 0) {
    is_running = false;
    return {};
  }

  // Check for end of show and loop.
  if (slide_number >= slide_show.slides_size()) {
    slide_number = 0;
  }

  auto slide = slide_show.slides(slide_number);
  auto left_millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now().time_since_epoch()) -
                     last_transition;
  double time_left = left_millis.count() / 1000.0;
  if (time_left > slide_show.delay()) {
    last_transition = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());
    ++slide_number;
  }
  return Image(FilesystemPath(slide.file_path()));
}

}  // namespace cszb_scoreboard
