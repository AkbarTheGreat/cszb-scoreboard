/*
ui/component/control/SlideshowSetup.cpp: This class is responsible for managing
a slideshow.  Typically, these are used for pre-show distractions.

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

#include "ui/component/control/SlideshowSetup.h"

namespace cszb_scoreboard {
class ScreenText;

auto SlideshowSetup ::Create(swx::Panel *wx)
    -> std::unique_ptr<SlideshowSetup> {
  auto control = std::make_unique<SlideshowSetup>(wx);
  control->initializeWidgets();
  return control;
}

void SlideshowSetup::createControls(Panel *control_panel) {
  main_panel = control_panel->panel();
  add_button = main_panel->button("Add New Slide");
  slide_panel = main_panel->panel();
  positionWidgets(control_panel);
  bindEvents();
}

void SlideshowSetup::positionWidgets(Panel *control_panel) {
  control_panel->addWidget(*main_panel, 0, 0);

  main_panel->addWidget(*add_button, 0, 0);
  main_panel->addWidgetWithSpan(*slide_panel, 1, 0, 1, 2);

  slide_panel->runSizer();
  main_panel->runSizer();
  control_panel->runSizer();
}

void SlideshowSetup::bindEvents() {}

void SlideshowSetup::updateScreenText(ScreenText *screen_text) {}

}  // namespace cszb_scoreboard
