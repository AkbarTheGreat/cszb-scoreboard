/*
ui/widget/Animation.h: A widget which renders an animated gif.

Copyright 2023-2025 Tracy Beck

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
#include "ui/widget/swx/MediaCtrl.h"

namespace cszb_scoreboard {

class Animation : public Widget {
 public:
  explicit Animation(swx::MediaCtrl *ctrl) { _wx = ctrl; }
  void load(const std::string &url) { auto test = _wx->Load(url); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::MediaCtrl *_wx;
};

}  // namespace cszb_scoreboard
