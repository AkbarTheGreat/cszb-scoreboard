/*
ui/component/control/ImageSearch.h: Handles searching for images from a webview for quick use as a bit.

Copyright 2022 Tracy Beck

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
#include "ui/widget/Label.h"

namespace cszb_scoreboard {
class PreviewPanel;

namespace swx {
class Panel;
}  // namespace swx

class ImageSearch : public ScreenImageController {
 public:
  ImageSearch(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenImageController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<ImageSearch>;

 private:
  std::unique_ptr<Panel> drop_target;
  std::unique_ptr<Label> drop_text;

  void bindEvents();
  void createControls(Panel *control_panel) override;
  void positionWidgets(Panel *control_panel) override;
};

}  // namespace cszb_scoreboard
