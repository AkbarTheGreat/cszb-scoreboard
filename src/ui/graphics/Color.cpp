/*
ui/Color.cpp: A wrapper around wxColour which gives convenience methods for
manipulating colors

Copyright 2019 Tracy Beck

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

// Calculations taken from
// https://stackoverflow.com/questions/1855884/determine-font-color-based-on-background-color
// as a way to calculate font color to contrast effectively against an arbitrary
// background.
Color Color::contrastColor() {
  int intensity = 0;

  // Counting the perceptive luminance - human eye favors green color...
  double luminance = (0.299 * red() + 0.587 * green() + 0.114 * blue()) / 255;

  if (luminance > 0.5)
    intensity = 0;  // bright colors - black font
  else
    intensity = 255;  // dark colors - white font

  return Color(intensity, intensity, intensity);
}

}  // namespace cszb_scoreboard
