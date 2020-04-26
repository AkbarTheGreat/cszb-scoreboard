/*
ui/component/control/ScoreControl.h: This class is responsible for generating a
score update which is sent to all scoreboard screens.

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

#include <wx/clrpicker.h>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include <vector>

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
  void colorChanged(wxColourPickerEvent &event);
  void createControls(wxPanel *control_panel) override;
  void homeUpdated(wxKeyEvent &event);
  void homeNameUpdated(wxKeyEvent &event);
  void homeAddOne(wxCommandEvent &event);
  void homeAddFive(wxCommandEvent &event);
  void homeMinusOne(wxCommandEvent &event);
  void positionWidgets(wxPanel *control_panel);
  void selectLogo(wxCommandEvent &event);
  void toggleIntroMode(wxCommandEvent &event);
  std::vector<proto::RenderableText> introLines(bool isHome);
  std::vector<proto::RenderableText> scoreLines(bool isHome);
  void updatePreview() override;

  wxTextCtrl *alpha_ctrl, *size_ctrl;
  wxToggleButton *team_intro_button;
  wxPanel *team_controls_panel;
  wxStaticText *home_score_label, *away_score_label;
  wxColourPickerCtrl *home_color_picker, *away_color_picker;
  wxTextCtrl *home_name_entry, *away_name_entry;
  wxTextCtrl *home_score_entry, *away_score_entry;
  wxPanel *home_button_panel, *away_button_panel;
  wxButton *home_plus_1, *home_plus_5, *home_minus_1;
  wxButton *away_plus_1, *away_plus_5, *away_minus_1;
  wxButton *home_logo_button, *away_logo_button;
  wxStaticText *home_logo_label, *away_logo_label;
  std::optional<wxImage> home_logo, away_logo;
};

}  // namespace cszb_scoreboard
