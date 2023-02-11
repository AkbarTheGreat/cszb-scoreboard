/*
ui/component/control/LocalImage.h: Handles loading images from disk or from the
clipboard.

Copyright 2019-2023 Tracy Beck

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

#include "ui/component/control/ScreenImageController.h"  // for ScreenImageC...
#include "ui/widget/Button.h"                            // for Button
#include "ui/widget/Panel.h"                             // for Panel

namespace cszb_scoreboard {

namespace swx {
class Panel;
}  // namespace swx

class LocalImage : public ScreenImageController {
 public:
  explicit LocalImage(swx::Panel *wx) : ScreenImageController(wx) {}
  static auto Create(swx::Panel *wx) -> std::unique_ptr<LocalImage>;

 private:
  std::unique_ptr<Panel> button_panel, inner_panel;
  std::unique_ptr<Button> browse_button, paste_button;

  void bindEvents();
  void createControls(Panel *control_panel) override;
  void positionWidgets(Panel *control_panel) override;
  void browsePressed();
  void pastePressed();
};

}  // namespace cszb_scoreboard
