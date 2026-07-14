/*
ui/widget/IndicatorButton.cpp: A button with an indicator stripe.

Copyright 2026 Tracy Beck

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

#include "ui/widget/IndicatorButton.h"

#include <wx/bitmap.h>  // for wxBitmap

#include "config/Position.h"       // for Size
#include "ui/widget/Image.h"       // for Image
#include "ui/widget/swx/Button.h"  // for Button
#include "wx/defs.h"               // for wxDirection
#include "wx/window.h"             // for wxWindow

namespace cszb_scoreboard {
class Color;

void IndicatorButton::setIndicatorColor(const Color& color) {
  // Create a 6x16 color block representing the stripe using the Image wrapper
  constexpr int STRIPE_WIDTH = 6;
  constexpr int STRIPE_HEIGHT = 16;
  Image image(Size{.width = STRIPE_WIDTH, .height = STRIPE_HEIGHT});
  image.color(color);
  wxBitmap bitmap(image.wx());

  auto* btn = dynamic_cast<swx::Button*>(wx());
  if (btn != nullptr) {
    btn->SetBitmap(bitmap);
    btn->SetBitmapPosition(wxLEFT);
  }
}

}  // namespace cszb_scoreboard
