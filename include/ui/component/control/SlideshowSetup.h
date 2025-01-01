/*
ui/component/control/SlideshowSetup.h: This class is responsible for managing a
slideshow.  Typically, these are used for pre-show distractions.

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

#include <memory>  // for unique_ptr
#include <vector>  // for vector

#include "ui/component/control/ScreenTextController.h"  // for ScreenTextCon...
#include "ui/component/control/slideshow/SlidePreview.h"  // for Slide
#include "ui/widget/Button.h"                             // for Button
#include "ui/widget/Panel.h"                              // for Panel

namespace cszb_scoreboard {
class ScreenText;

namespace swx {
class Panel;
}  // namespace swx

class SlideshowSetup : public ScreenTextController {
 public:
  explicit SlideshowSetup(swx::Panel *wx)
      : ScreenTextController(wx,
                             /*display_update_screens=*/false) {}
  static auto Create(swx::Panel *wx) -> std::unique_ptr<SlideshowSetup>;

 private:
  std::unique_ptr<Button> add_button;
  std::unique_ptr<Panel> main_panel, slide_panel;
  std::vector<std::unique_ptr<SlidePreview>> slide_previews;

  void createControls(Panel *control_panel) override;
  void updateScreenText(ScreenText *screen_text) override;
  void bindEvents();
  void positionWidgets(Panel *control_panel);
  void addNewSlide();
};

}  // namespace cszb_scoreboard
