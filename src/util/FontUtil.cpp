/*
util/FontUtil.cpp: Utilities for dealing with variable sized fonts in the
scoreboard.

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

#include "util/FontUtil.h"

#include "config/Position.h"  // for Size

namespace cszb_scoreboard {

const int SCALE_FACTOR = 75;

auto FontUtil::scaleFactor(const Size& display_size, int32_t font_size)
    -> int32_t {
  return scaleFactor(display_size, static_cast<double>(font_size));
}

auto FontUtil::scaleFactor(const Size& display_size, double font_size)
    -> int32_t {
  int32_t scaled_size = display_size.height * font_size / SCALE_FACTOR;
  if (scaled_size == 0) {
    scaled_size = 1;
  }
  return scaled_size;
}

}  // namespace cszb_scoreboard