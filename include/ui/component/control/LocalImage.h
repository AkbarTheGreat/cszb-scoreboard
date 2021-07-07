/*
ui/component/control/LocalImage.h: Handles loading images from disk or from the
clipboard.

Copyright 2019-2021 Tracy Beck

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

#include <wx/wx.h>

#include "config.pb.h"
#include "ui/component/control/ScreenImageController.h"
#include "ui/widget/Button.h"
#include "ui/widget/Panel.h"

namespace cszb_scoreboard {

class LocalImage : public ScreenImageController {
 public:
  LocalImage(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenImageController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<LocalImage>;

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
