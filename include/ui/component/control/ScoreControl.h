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

#include "config.pb.h"
#include "ui/component/control/ScreenTextController.h"

namespace cszb_scoreboard {

class ScoreControl : public ScreenTextController {
 public:
  static ScoreControl *Create(PreviewPanel *preview_panel, wxWindow *parent);

 private:
  ScoreControl(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  void addToEntry(wxTextCtrl *entry, int amount);
  void awayUpdated(wxKeyEvent &event);
  void awayNameUpdated(wxKeyEvent &event);
  void awayAddOne(wxCommandEvent &event);
  void awayAddFive(wxCommandEvent &event);
  void awayMinusOne(wxCommandEvent &event);
  void bindEvents();
  void createControls(wxPanel *control_panel) override;
  void homeUpdated(wxKeyEvent &event);
  void homeNameUpdated(wxKeyEvent &event);
  void homeAddOne(wxCommandEvent &event);
  void homeAddFive(wxCommandEvent &event);
  void homeMinusOne(wxCommandEvent &event);
  void positionWidgets(wxPanel *control_panel);
  void updatePreview() override;

  wxStaticText *home_score_label;
  wxTextCtrl *home_name_entry;
  wxTextCtrl *home_score_entry;
  wxPanel *home_button_panel;
  wxButton *home_plus_1;
  wxButton *home_plus_5;
  wxButton *home_minus_1;
  wxStaticText *away_score_label;
  wxTextCtrl *away_name_entry;
  wxTextCtrl *away_score_entry;
  wxPanel *away_button_panel;
  wxButton *away_plus_1;
  wxButton *away_plus_5;
  wxButton *away_minus_1;
};

}  // namespace cszb_scoreboard