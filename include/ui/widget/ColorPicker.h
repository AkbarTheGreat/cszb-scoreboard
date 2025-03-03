/*
ui/widget/ColorPicker.h: A widget which allows selecting colors from a palatte
or from a color wheel.

Copyright 2021-2025 Tracy Beck

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

#include "ui/graphics/Color.h"
#include "ui/widget/Widget.h"
#include "ui/widget/swx/ColourPickerCtrl.h"

namespace cszb_scoreboard {

class ColorPicker : public Widget {
 public:
  explicit ColorPicker(swx::ColourPickerCtrl *picker) { _wx = picker; }

  void bind(const wxEventTypeTag<wxColourPickerEvent> &eventType,
            const std::function<void(wxColourPickerEvent &)> &lambda,
            int id = wxID_ANY) {
    wx()->Bind(eventType, lambda, id);
  }
  auto color() -> Color { return Color(_wx->GetColour()); }
  void setColor(const Color &color) { _wx->SetColour(color); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::ColourPickerCtrl *_wx;
};

}  // namespace cszb_scoreboard
