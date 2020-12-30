/*
ui/Color.cpp: A wrapper around wxColour which gives convenience methods for
manipulating colors

Copyright 2019-2020 Tracy Beck

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

#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

// Factors for calculating perceptive luminance - the human eye favors green
// colors. (see Color::contrastColor for the source of numbers)
static const float RED_LUMINANCE_FACTOR = 0.299;
static const float GREEN_LUMINANCE_FACTOR = 0.587;
static const float BLUE_LUMINANCE_FACTOR = 0.114;

static const int BYTE_SIZE = 255;

static const float LUMINANCE_THRESHOLD = 0.5;

// Calculations taken from
// https://stackoverflow.com/questions/1855884/determine-font-color-based-on-background-color
// as a way to calculate font color to contrast effectively against an arbitrary
// background.
auto Color::contrastColor() -> Color {
  int intensity = 0;

  double luminance =
      (RED_LUMINANCE_FACTOR * red() + GREEN_LUMINANCE_FACTOR * green() +
       BLUE_LUMINANCE_FACTOR * blue()) /
      BYTE_SIZE;

  if (luminance > LUMINANCE_THRESHOLD) {
    intensity = 0;  // bright colors - black font
  } else {
    intensity = BYTE_SIZE;  // dark colors - white font
  }

  return Color(intensity, intensity, intensity);
}

}  // namespace cszb_scoreboard
