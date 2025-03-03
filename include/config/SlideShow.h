/*
config/SlideShow.h: This class is a singleton which
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
#pragma once

#include <stdint.h>  // for int32_t

#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "config/swx/image.h"  // for Image
#include "slide_show.pb.h"     // for SlideShow, SlideInfo (ptr only)
#include "util/Singleton.h"    // for Singleton, SingletonClass

namespace cszb_scoreboard {
class FilesystemPath;

constexpr double DEFAULT_SLIDESHOW_DELAY = 5.0f;

class SlideShow {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit SlideShow(SingletonClass c)
      : SlideShow(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void swapSlides(int32_t a, int32_t b);
  void removeSlide(int32_t index);
  void addSlide(const std::string &name, const FilesystemPath &file,
                int32_t index = -1);
  void saveShow();
  auto slides(int32_t start, int32_t end) -> std::vector<proto::SlideInfo>;
  auto delay() -> double {
    return slide_show.delay() > 0 ? slide_show.delay()
                                  : DEFAULT_SLIDESHOW_DELAY;
  }
  void setDelay(double delay) { slide_show.set_delay(delay); }
  auto isRunning() -> bool { return is_running; }
  void start();
  void stop();
  auto nextSlide() -> Image;

  PUBLIC_TEST_ONLY
  SlideShow(SingletonClass c, Singleton *singleton);

 private:
  proto::SlideShow slide_show;
  Singleton *singleton;
  int32_t slide_number = 0;
  bool is_running = false;
  std::chrono::seconds last_transition;
};
}  // namespace cszb_scoreboard
