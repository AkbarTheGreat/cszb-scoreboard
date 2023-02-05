/*
ui/widget/Toggle.h: A button which toggles between two states.

Copyright 2021-2023 Tracy Beck

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

#include "ui/widget/Widget.h"
#include "ui/widget/swx/ToggleButton.h"

namespace cszb_scoreboard {

class Toggle : public Widget {
 public:
  explicit Toggle(swx::ToggleButton *button) { _wx = button; }

  void setLabel(const std::string &label) { _wx->SetLabelText(label); }
  auto value() -> bool { return _wx->GetValue(); }
  void toggle() { _wx->SetValue(!_wx->GetValue()); }
  void press() { _wx->SetValue(true); }
  void unpress() { _wx->SetValue(false); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::ToggleButton *_wx;
};

}  // namespace cszb_scoreboard
