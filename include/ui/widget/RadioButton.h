/*
ui/widget/RadioButton.h: A single round (radio) button.

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

#include <wx/gbsizer.h>

#include "ui/widget/Widget.h"
#include "ui/widget/swx/RadioButton.h"

namespace cszb_scoreboard {

class RadioButton : public Widget {
 public:
  explicit RadioButton(swx::RadioButton *radio) { _wx = radio; }

  auto selected() -> bool { return _wx->GetValue(); };
  void setSelected(bool sel) { _wx->SetValue(sel); };

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::RadioButton *_wx;
};

}  // namespace cszb_scoreboard
