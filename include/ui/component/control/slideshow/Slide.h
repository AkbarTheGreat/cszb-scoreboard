/*
ui/component/control/slideshow/Slide.h: The UI for a given slide in a slideshow,
including buttons to modify or move the slide around.

Copyright 2024 Tracy Beck

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

#include "ui/component/control/ImagePreview.h"
#include "ui/widget/Button.h"  // for Button
#include "ui/widget/Label.h"   // for Label
#include "ui/widget/Panel.h"
#include "util/FilesystemPath.h"

namespace cszb_scoreboard {

class Slide : public Panel {
 public:
  explicit Slide(swx::Panel *wx, FilesystemPath image);

 private:
  std::unique_ptr<ImagePreview> slide_preview;
  std::unique_ptr<Label> slide_name;
  std::unique_ptr<Button> left_button;
  std::unique_ptr<Button> right_button;
  std::unique_ptr<Button> remove_button;

  void bindEvents();
  void positionWidgets();
};

}  // namespace cszb_scoreboard
