/*
ui/widget/Radio.h: A collection of radio buttons.

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

#include <wx/gbsizer.h>

#include "ui/widget/Widget.h"
#include "ui/widget/swx/RadioBox.h"

namespace cszb_scoreboard {

class Radio : public Widget {
 public:
  explicit Radio(swx::RadioBox *radio) {
    _wx = radio;
    _wx->SetSelection(0);
  }
  auto selection() -> int { return _wx->GetSelection(); }
  void setSelection(int sel) { _wx->SetSelection(sel); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::RadioBox *_wx;
};

}  // namespace cszb_scoreboard
