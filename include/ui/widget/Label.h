/*
ui/widget/Label.h: A static text label.

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

#include <wx/string.h>

#include <string>

#include "ui/widget/Widget.h"
#include "ui/widget/swx/StaticText.h"

class wxWindow;

namespace cszb_scoreboard {

class Label : public Widget {
 public:
  explicit Label(swx::StaticText *label) { wx = label; }
  void set(const std::string &label) { wx->SetLabelText(label); }
  void setWithHotkey(const std::string &label) { wx->SetLabel(label); }
  auto text() -> std::string { return wx->GetLabelText().ToStdString(); }
  void bold(bool is_bold);

 protected:
  auto _wx() const -> wxWindow * override { return wx; }

 private:
  swx::StaticText *wx;
};

}  // namespace cszb_scoreboard
