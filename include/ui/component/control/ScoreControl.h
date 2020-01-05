/*
ui/component/control/ScoreControl.h: This class is responsible for generating a
score update which is sent to all scoreboard screens.

Copyright 2019 Tracy Beck

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

#include "proto/config.pb.h"
#include "ui/component/control/ScreenTextController.h"

namespace cszb_scoreboard {

class ScoreControl : public ScreenTextController {
 public:
  static ScoreControl *Create(PreviewPanel *preview_panel, wxWindow *parent);

 private:
  ScoreControl(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  void awayUpdated(wxKeyEvent &event);
  void bindEvents();
  void createControls(wxPanel *control_panel) override;
  void homeUpdated(wxKeyEvent &event);
  void positionWidgets(wxPanel *control_panel);
  void updatePreview() override;
  proto::ScreenSide updateSide() override;

  wxStaticText *home_score_label;
  wxTextCtrl *home_score_entry;
  wxStaticText *away_score_label;
  wxTextCtrl *away_score_entry;
};

}  // namespace cszb_scoreboard
