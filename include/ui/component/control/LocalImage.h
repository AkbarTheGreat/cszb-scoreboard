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
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"

namespace cszb_scoreboard {

class LocalImage : public ScreenTextController {
 public:
  static LocalImage *Create(PreviewPanel *preview_panel, wxWindow *parent);

 private:
  wxButton *browse_button;
  wxPanel *button_panel;
  wxStaticText *current_file;
  wxButton *paste_button;
  wxPanel *inner_panel;
  wxImage all_screen_image, home_screen_image, away_screen_image;
  std::string all_screen_filename, home_screen_filename, away_screen_filename;
  TeamSelector *screen_selection;

  LocalImage(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  void bindEvents();
  void browsePressed(wxCommandEvent &event);
  void createControls(wxPanel *control_panel) override;
  void pastePressed(wxCommandEvent &event);
  void positionWidgets(wxPanel *control_panel);
  void screenChanged(wxCommandEvent &event);
  void updatePreview() override;
};

}  // namespace cszb_scoreboard
