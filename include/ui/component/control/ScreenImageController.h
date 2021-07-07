/*
ui/component/control/ScreenImageController.h: A specialized ScreenTextController
which handles sending images loaded from elsewhere, as opposed to a static
background.  By default, this also does not support any text at all.  This is
not intended to be used standalone, or as a polymorphic container pointer, but
merely a way to abstract away common functionality.

Copyright 2020-2021 Tracy Beck

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
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"
#include "ui/widget/Label.h"

namespace cszb_scoreboard {

class ScreenImageController : public ScreenTextController {
 protected:
  std::unique_ptr<Label> current_image_label;
  std::unique_ptr<TeamSelector> screen_selection;
  wxImage all_screen_image, home_screen_image, away_screen_image;
  std::string all_screen_image_name, home_screen_image_name,
      away_screen_image_name;

  ScreenImageController(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenTextController(preview_panel, wx) {}
  void bindEvents();
  void createControls(Panel *control_panel) override;
  virtual void positionWidgets(Panel *control_panel) = 0;
  void screenChanged();
  void updateScreenText(ScreenText *screen_text) override;
};

}  // namespace cszb_scoreboard
