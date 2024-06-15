/*
ui/widget/Button.h: A simple button, nothing super fancy.

Copyright 2021-2024 Tracy Beck

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
#include "ui/widget/swx/Button.h"

namespace cszb_scoreboard {

class Button : public Widget {
 public:
  explicit Button(swx::Button *button) { _wx = button; }

  void disable() { wx()->Disable(); }
  void enable() { wx()->Enable(); }
  auto id() -> int { return wx()->GetId(); }
  void setText(const std::string &text) { wx()->SetLabel(text); }
  void toolTip(const std::string &tip) { wx()->SetToolTip(tip); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::Button *_wx;
};

}  // namespace cszb_scoreboard
