/*
ui/component/control/ImageSearch.h: Handles searching for images from a webview
for quick use as a bit.

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
#include "ui/widget/Browser.h"
#include "ui/widget/Button.h"  // for Button
#include "ui/widget/DragAndDropHandler.h"
#include "ui/widget/Label.h"
#include "ui/widget/Panel.h"  // for Panel

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
  std::unique_ptr<Panel> inner_panel, drop_target, reset_button_panel;
  std::unique_ptr<Label> drop_text;
  std::unique_ptr<Browser> browser;
  std::unique_ptr<Button> reset_button;
  std::unique_ptr<DragAndDropHandler> drag_handler;

  void bindEvents();
  void createControls(Panel *control_panel) override;
  void positionWidgets(Panel *control_panel) override;
  void onURLDrop(const std::string &url);
  void resetURL();
};

}  // namespace cszb_scoreboard
