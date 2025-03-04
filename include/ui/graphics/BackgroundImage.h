/*
ui/BackgroundImage.h: This class builds an image to serve as the background
for ScreenText

Copyright 2019-2025 Tracy Beck

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

#include <climits>  // for UCHAR_MAX

#include "config/Position.h"   // for Size
#include "config/swx/image.h"  // for Image

namespace cszb_scoreboard {
class Color;

class BackgroundImage : public Image {
 public:
  BackgroundImage(::cszb_scoreboard::Size size, const Color &color,
                  unsigned char alpha);
  BackgroundImage(::cszb_scoreboard::Size size, const Color &color)
      : BackgroundImage(size, color, UCHAR_MAX) {}
  static auto errorImage(::cszb_scoreboard::Size size) -> BackgroundImage;
};

}  // namespace cszb_scoreboard
