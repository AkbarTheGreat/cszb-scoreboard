/*
ui/component/control/ScoreControl.cpp: This class is responsible for generating
a score update which is sent to all scoreboard screens.

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

#include "ui/component/control/ScoreControl.h"

namespace cszb_scoreboard {

ScoreControl *ScoreControl::Create(PreviewPanel *preview_panel,
                                   wxWindow *parent) {
  ScoreControl *control = new ScoreControl(preview_panel, parent);
  control->initializeWidgets();
  return control;
}

void ScoreControl::createControls(wxPanel *control_panel) {
  home_score_label = new wxStaticText(control_panel, wxID_ANY, wxT("Home"));
  home_score_entry = new wxTextCtrl(control_panel, wxID_ANY, wxT("0"));

  away_score_label = new wxStaticText(control_panel, wxID_ANY, wxT("Away"));
  away_score_entry = new wxTextCtrl(control_panel, wxID_ANY, wxT("0"));

  positionWidgets(control_panel);
  bindEvents();
}

void ScoreControl::bindEvents() {}

void ScoreControl::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 2, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  sizer->Add(home_score_label, 0, wxALL, 5);
  sizer->Add(away_score_label, 0, wxALL, 5);
  sizer->Add(home_score_entry, 0, wxALL, 5);
  sizer->Add(away_score_entry, 0, wxALL, 5);

  control_panel->SetSizerAndFit(sizer);
}

void ScoreControl::updatePreview() {}

proto::ScreenSide ScoreControl::updateSide() { return proto::ScreenSide(); }

}  // namespace cszb_scoreboard
