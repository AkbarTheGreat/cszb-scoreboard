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

#include <vector>

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "slide_show.pb.h"
#include "util/FilesystemPath.h"
#include "util/Singleton.h"  // for Singleton, SingletonClass

namespace cszb_scoreboard {

class SlideShow {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit SlideShow(SingletonClass c)
      : SlideShow(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  void swapSlides(uint32_t a, uint32_t b);
  void removeSlide(uint32_t index);
  void addSlide(const std::string &name, const FilesystemPath &file,
                int32_t index = -1);
  void saveShow();
  auto slides(int32_t start, int32_t end) -> std::vector<proto::SlideInfo>;

  PUBLIC_TEST_ONLY
  SlideShow(SingletonClass c, Singleton *singleton);

 private:
  proto::SlideShow slide_show;
  Singleton *singleton;
};
}  // namespace cszb_scoreboard
