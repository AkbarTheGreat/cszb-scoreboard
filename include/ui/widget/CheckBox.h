/*
ui/widget/CheckBox.h: A single checkbox button.

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

#include "ui/widget/Widget.h"
#include "ui/widget/swx/CheckBox.h"

namespace cszb_scoreboard {

class CheckBox : public Widget {
 public:
  explicit CheckBox(swx::CheckBox *check) { _wx = check; }

  [[nodiscard]] auto checked() const -> bool { return _wx->GetValue(); }
  void setChecked(bool checked) { _wx->SetValue(checked); }
  void toolTip(const std::string &tip) { wx()->SetToolTip(tip); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::CheckBox *_wx;
};

}  // namespace cszb_scoreboard
