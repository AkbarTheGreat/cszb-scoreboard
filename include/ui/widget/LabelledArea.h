/*
ui/widget/LabelledArea.h: A labelled area is a border with an in-built label,
for grouping chunks of UI together.  To embed things inside of this object,
reference it's holds() method.

Copyright 2022-2024 Tracy Beck

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

#include <memory>  // for unique_ptr

#include "ui/widget/Panel.h"          // for Panel
#include "ui/widget/Widget.h"         // for Widget
#include "ui/widget/swx/StaticBox.h"  // for StaticBoxImpl, StaticBox

class wxWindow;

namespace cszb_scoreboard {

class LabelledArea : public Widget {
 public:
  explicit LabelledArea(swx::StaticBox *static_box);
  [[nodiscard]] auto holds() -> Panel * { return held_panel.get(); }

 private:
  [[nodiscard]] auto wx() const -> wxWindow * override {
    return dynamic_cast<swx::StaticBoxImpl *>(_wx);
  }

  std::unique_ptr<Panel> held_panel;

 protected:
  swx::StaticBox *_wx;
};

}  // namespace cszb_scoreboard
