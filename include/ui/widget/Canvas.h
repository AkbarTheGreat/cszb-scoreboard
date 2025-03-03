/*
ui/widget/Panel.h: A canvas represents an interior panel which exists primarily
to be drawn upon.

Copyright 2022-2025 Tracy Beck

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

#include "ui/widget/Panel.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class Canvas : public Panel {
 public:
  explicit Canvas(swx::Panel *panel) : Panel(panel) {
    panel->setBackgroundStyle(wxBG_STYLE_PAINT);
  }
  ~Canvas() override = default;
};

}  // namespace cszb_scoreboard
