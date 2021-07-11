/*
ui/widget/CheckBox.h: A single checkbox button.

Copyright 2021 Tracy Beck

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
  explicit CheckBox(swx::CheckBox *check) { wx = check; }

  auto checked() -> bool { return wx->GetValue(); }
  void setChecked(bool checked) { wx->SetValue(checked); }

 protected:
  auto _wx() const -> wxWindow * override { return wx; }

 private:
  swx::CheckBox *wx;
};

}  // namespace cszb_scoreboard
