/*
ui/widget/LabelledArea.h: A labelled area is a border with an in-built label,
for grouping chunks of UI together.  To embed things inside of this object,
reference it's holds() method.

Copyright 2022-2023 Tracy Beck

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

#include "ui/widget/LabelledArea.h"

#include <cstdint>  // for int32_t

#include "ui/widget/Panel.h"      // for Panel
#include "ui/widget/swx/Panel.h"  // for PanelImpl

namespace cszb_scoreboard {

// To keep the held panel from intruding on the label in the frame, we need some
// built-in buffer.
constexpr int32_t BUFFER_SIZE = 15;

LabelledArea::LabelledArea(swx::StaticBox *static_box) {
  _wx = static_box;
  held_panel = std::make_unique<Panel>(new swx::PanelImpl(wx()));
  addWidget(*held_panel, 0, 0, BUFFER_SIZE);
}

}  // namespace cszb_scoreboard
