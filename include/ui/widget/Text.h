/*
ui/widget/Text.h: A text entry box.

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
#include "ui/widget/swx/TextCtrl.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

class Text : public Widget {
 public:
  explicit Text(swx::TextCtrl *label) { _wx = label; }

  void clear() { _wx->Clear(); }
  void disable() { wx()->Disable(); }
  void enable() { wx()->Enable(); }
  void focus() { wx()->SetFocus(); }
  void setValue(const std::string &value) { _wx->SetValue(value); }
  void setValue(int value) { _wx->SetValue(StringUtil::intToString(value)); }
  auto value() -> std::string { return _wx->GetValue().ToStdString(); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::TextCtrl *_wx;
};

}  // namespace cszb_scoreboard
