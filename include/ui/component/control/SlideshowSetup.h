/*
ui/component/control/SlideshowSetup.h: This class is responsible for managing a
slideshow.  Typically, these are used for pre-show distractions.

Copyright 2024-2025 Tracy Beck

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

#include <stdint.h>  // for int32_t

#include <memory>  // for unique_ptr
#include <vector>  // for vector

#include "ui/component/control/ScreenTextController.h"    // for ScreenTextC...
#include "ui/component/control/slideshow/SlidePreview.h"  // for SlidePreview
#include "ui/widget/Button.h"                             // for Button
#include "ui/widget/Label.h"                              // for Label
#include "ui/widget/Panel.h"                              // for Panel
#include "ui/widget/Text.h"                               // for Text

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
  void moveSlideLeft(int32_t index);
  void moveSlideRight(int32_t index);
  void removeSlide(int32_t index);

 private:
  int current_slide_page = 0;
  std::unique_ptr<Button> add_button, previous_page, next_page;
  std::unique_ptr<Button> start_button, stop_button;
  std::unique_ptr<Panel> main_panel, config_panel, slide_panel, delay_panel;
  std::unique_ptr<Label> delay_label;
  std::unique_ptr<Text> delay_setting;
  std::vector<std::unique_ptr<SlidePreview>> slide_previews;

  void createControls(Panel *control_panel) override;
  void updateScreenText(ScreenText *screen_text) override;
  void bindEvents();
  void positionWidgets(Panel *control_panel);
  void addNewSlide();
  void setSlidePreviews(unsigned int page_number);
  void swapSlides(int32_t a, int32_t b);
  void previousPage();
  void nextPage();
  void lastPage();
  void startShow();
  void stopShow();
  void delayUpdated();
};

}  // namespace cszb_scoreboard
