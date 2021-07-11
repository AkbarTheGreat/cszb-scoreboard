/*
ui/widget/Divider.h: A line which can be rendered to show a division in a UI.

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
#include "ui/widget/swx/StaticLine.h"

namespace cszb_scoreboard {

class Divider : public Widget {
 public:
  explicit Divider(swx::StaticLine *line) { wx = line; }

 protected:
  auto _wx() const -> wxWindow * override { return wx; }

 private:
  swx::StaticLine *wx;
};

}  // namespace cszb_scoreboard
