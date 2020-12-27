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
  static auto Create(PreviewPanel *preview_panel, wxWindow *parent)
      -> ScoreControl *;

 private:
  ScoreControl(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  void addHomeAwayWidgetPair(wxSizer *sizer, wxWindow *home_widget,
                             wxWindow *away_widget);
  void addToEntry(wxTextCtrl *entry, int amount);
  void bindEvents();
  void createControls(wxPanel *control_panel) override;
  void positionWidgets(wxPanel *control_panel);
  auto introLines(bool isHome) -> std::vector<proto::RenderableText>;
  auto scoreLines(bool isHome) -> std::vector<proto::RenderableText>;
  void updateScreenText(ScreenText *screen_text) override;
  // wxWidgets callbacks, waive linting error for references.
  void awayUpdated(wxKeyEvent &event);      // NOLINT(google-runtime-references)
  void awayNameUpdated(wxKeyEvent &event);  // NOLINT(google-runtime-references)
  void awayAddOne(wxCommandEvent &event);   // NOLINT(google-runtime-references)
  void awayAddFive(wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void awayMinusOne(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void colorChanged(
      wxColourPickerEvent &event);          // NOLINT(google-runtime-references)
  void homeUpdated(wxKeyEvent &event);      // NOLINT(google-runtime-references)
  void homeNameUpdated(wxKeyEvent &event);  // NOLINT(google-runtime-references)
  void homeAddOne(wxCommandEvent &event);   // NOLINT(google-runtime-references)
  void homeAddFive(wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void homeMinusOne(
      wxCommandEvent &event);              // NOLINT(google-runtime-references)
  void selectLogo(wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void toggleIntroMode(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)

  std::optional<wxImage> home_logo, away_logo;
  // All of the following raw pointers are to wxWidgets objects which wxWidgets
  // will own and handle destruction of.
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
};

}  // namespace cszb_scoreboard
