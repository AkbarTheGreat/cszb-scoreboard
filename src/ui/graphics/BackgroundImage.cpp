
/*
ui/BackgroundImage.cpp: This class builds an image to serve as the background
for ScreenText

Copyright 2019-2024 Tracy Beck

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

#include "ui/graphics/BackgroundImage.h"

#include <wx/gdicmn.h>  // for wxRect

#include <cstdlib>  // for malloc, size_t
#include <cstring>  // for memset

#include "config/Position.h"    // for Size
#include "ui/graphics/Color.h"  // for Color

namespace cszb_scoreboard {

const int NUMBER_OF_SQUARES_HIGH = 8;

BackgroundImage::BackgroundImage(::cszb_scoreboard::Size size,
                                 const Color& color, unsigned char alpha)
    : Image(size, true) {
  wxRect fullMask(0, 0, size.width, size.height);
  SetRGB(fullMask, color.red(), color.green(), color.blue());
  size_t ptr_size = size.width * size.height;
  auto* alpha_ptr = static_cast<unsigned char*>(malloc(ptr_size));
  if (alpha_ptr != nullptr) {
    memset(alpha_ptr, alpha, ptr_size);
    InitAlpha();
    SetAlpha(alpha_ptr);
  }
}

auto BackgroundImage::errorImage(::cszb_scoreboard::Size size)
    -> BackgroundImage {
  BackgroundImage error(size, Color("Red"));
  int square_size = size.height / NUMBER_OF_SQUARES_HIGH;
  Color white("White");
  bool is_red = true;
  for (int x = 0; x < size.width; ++x) {
    bool x_toggle = static_cast<bool>((x / square_size) & 1);
    for (int y = 0; y < size.height; ++y) {
      bool y_toggle = static_cast<bool>((y / square_size) & 1);
      if (x_toggle ^ y_toggle) {
        error.SetRGB(x, y, white.red(), white.green(), white.blue());
      }
    }
  }
  return error;
}

}  // namespace cszb_scoreboard
