/*
ui/widget/DropDown.h: A drop-down selector to pick one from many strings.

Copyright 2023-2024 Tracy Beck

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

#include <cstdint>  // for int32_t
#include <string>   // for string
#include <vector>   // for vector

#include "ui/widget/Widget.h"      // for Widget
#include "ui/widget/swx/Choice.h"  // for Choice
#include "wx/window.h"             // for wxWindow

namespace cszb_scoreboard {

class DropDown : public Widget {
 public:
  DropDown(swx::Choice *choice, const std::vector<std::string> &choices);

  auto selected() -> std::string;
  void setSelected(int32_t selection);
  void toolTip(const std::string &tip) { wx()->SetToolTip(tip); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::Choice *_wx;
};

}  // namespace cszb_scoreboard
