/*
ui/component/control/LocalImage.h: Handles loading images from disk or from the
clipboard.

Copyright 2019-2020 Tracy Beck

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
#include "ui/component/control/TeamSelector.h"

namespace cszb_scoreboard {

class LocalImage : public ScreenImageController {
 public:
  static LocalImage *Create(PreviewPanel *preview_panel, wxWindow *parent);

 private:
  wxButton *browse_button;
  wxPanel *button_panel;
  wxButton *paste_button;
  wxPanel *inner_panel;

  LocalImage(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenImageController(preview_panel, parent) {}
  void bindEvents();
  void browsePressed(wxCommandEvent &event);
  void createControls(wxPanel *control_panel) override;
  void pastePressed(wxCommandEvent &event);
  void positionWidgets(wxPanel *control_panel) override;
};

}  // namespace cszb_scoreboard
