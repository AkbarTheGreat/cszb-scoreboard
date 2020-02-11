/*
ui/component/control/ScoreControl.cpp: This class is responsible for generating
a score update which is sent to all scoreboard screens.

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

#include "ui/component/control/ScoreControl.h"

#include "config/TeamConfig.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int SCORE_FONT_SIZE = 20;
const int TEAM_FONT_SIZE = 10;
const int BORDER_SIZE = 3;

ScoreControl *ScoreControl::Create(PreviewPanel *preview_panel,
                                   wxWindow *parent) {
  ScoreControl *control = new ScoreControl(preview_panel, parent);
  control->initializeWidgets();
  return control;
}

void ScoreControl::createControls(wxPanel *control_panel) {
  // TODO: Populate these from settings-based defaults
  proto::ScreenSide side;
  side.set_home(true);

  home_score_label = new wxStaticText(control_panel, wxID_ANY, wxT("Home"));
  home_color_picker = new wxColourPickerCtrl(
      control_panel, wxID_ANY, TeamConfig::getInstance()->teamColor(side)[0]);
  home_name_entry = new wxTextCtrl(control_panel, wxID_ANY, wxT("Home Team"));
  home_score_entry = new wxTextCtrl(control_panel, wxID_ANY, wxT("0"));

  home_button_panel = new wxPanel(control_panel);
  home_plus_1 = new wxButton(home_button_panel, wxID_ANY, "+1",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  home_plus_5 = new wxButton(home_button_panel, wxID_ANY, "+5",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  home_minus_1 = new wxButton(home_button_panel, wxID_ANY, "-1",
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

  side.set_home(false);
  side.set_away(true);

  away_score_label = new wxStaticText(control_panel, wxID_ANY, wxT("Away"));
  away_color_picker = new wxColourPickerCtrl(
      control_panel, wxID_ANY, TeamConfig::getInstance()->teamColor(side)[0]);

  away_name_entry = new wxTextCtrl(control_panel, wxID_ANY, wxT("Away Team"));
  away_score_entry = new wxTextCtrl(control_panel, wxID_ANY, wxT("0"));

  away_button_panel = new wxPanel(control_panel);
  away_plus_1 = new wxButton(away_button_panel, wxID_ANY, "+1",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  away_plus_5 = new wxButton(away_button_panel, wxID_ANY, "+5",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  away_minus_1 = new wxButton(away_button_panel, wxID_ANY, "-1",
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

  positionWidgets(control_panel);
  bindEvents();
}

void ScoreControl::bindEvents() {
  home_score_entry->Bind(wxEVT_KEY_UP, &ScoreControl::homeUpdated, this);
  home_name_entry->Bind(wxEVT_KEY_UP, &ScoreControl::homeNameUpdated, this);
  away_score_entry->Bind(wxEVT_KEY_UP, &ScoreControl::awayUpdated, this);
  away_name_entry->Bind(wxEVT_KEY_UP, &ScoreControl::awayNameUpdated, this);
  home_plus_1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScoreControl::homeAddOne,
                    this);
  home_plus_5->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScoreControl::homeAddFive,
                    this);
  home_minus_1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScoreControl::homeMinusOne,
                     this);
  away_plus_1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScoreControl::awayAddOne,
                    this);
  away_plus_5->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScoreControl::awayAddFive,
                    this);
  away_minus_1->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScoreControl::awayMinusOne,
                     this);
  home_color_picker->Bind(wxEVT_COLOURPICKER_CHANGED,
                          &ScoreControl::colorChanged, this);
  away_color_picker->Bind(wxEVT_COLOURPICKER_CHANGED,
                          &ScoreControl::colorChanged, this);
}

void ScoreControl::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 2, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  sizer->Add(home_score_label, 0, wxALL, BORDER_SIZE);
  sizer->Add(away_score_label, 0, wxALL, BORDER_SIZE);
  sizer->Add(home_color_picker, 0, wxALL, BORDER_SIZE);
  sizer->Add(away_color_picker, 0, wxALL, BORDER_SIZE);
  sizer->Add(home_name_entry, 0, wxALL, BORDER_SIZE);
  sizer->Add(away_name_entry, 0, wxALL, BORDER_SIZE);
  sizer->Add(home_score_entry, 0, wxALL, BORDER_SIZE);
  sizer->Add(away_score_entry, 0, wxALL, BORDER_SIZE);

  wxFlexGridSizer *home_panel_sizer = new wxFlexGridSizer(1, 0, 0, 0);
  home_panel_sizer->Add(home_plus_1, 0, wxALL, BORDER_SIZE);
  home_panel_sizer->Add(home_plus_5, 0, wxALL, BORDER_SIZE);
  home_panel_sizer->Add(home_minus_1, 0, wxALL, BORDER_SIZE);
  home_button_panel->SetSizerAndFit(home_panel_sizer);
  sizer->Add(home_button_panel, 0, wxALL, BORDER_SIZE);

  wxFlexGridSizer *away_panel_sizer = new wxFlexGridSizer(1, 0, 0, 0);
  away_panel_sizer->Add(away_plus_1, 0, wxALL, BORDER_SIZE);
  away_panel_sizer->Add(away_plus_5, 0, wxALL, BORDER_SIZE);
  away_panel_sizer->Add(away_minus_1, 0, wxALL, BORDER_SIZE);
  away_button_panel->SetSizerAndFit(away_panel_sizer);
  sizer->Add(away_button_panel, 0, wxALL, BORDER_SIZE);

  control_panel->SetSizerAndFit(sizer);
}

void ScoreControl::updatePreview() {
  proto::ScreenSide home_side;
  home_side.set_home(true);
  proto::ScreenSide away_side;
  away_side.set_away(true);

  std::vector<proto::RenderableText> home_update;
  home_update.push_back(proto::RenderableText());
  home_update.back().set_text(home_score_entry->GetValue());
  home_update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  home_update.push_back(proto::RenderableText());
  home_update.back().set_text(home_name_entry->GetValue());
  home_update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  home_update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  std::vector<proto::RenderableText> away_update;
  away_update.push_back(proto::RenderableText());
  away_update.back().set_text(away_score_entry->GetValue());
  away_update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  away_update.push_back(proto::RenderableText());
  away_update.back().set_text(away_name_entry->GetValue());
  away_update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  away_update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  previewPanel()->setTextForPreview(home_update, home_color_picker->GetColour(),
                                    home_side);
  previewPanel()->setTextForPreview(away_update, away_color_picker->GetColour(),
                                    away_side);
}

void ScoreControl::homeUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::homeNameUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::awayUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::awayNameUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::colorChanged(wxColourPickerEvent &event) { updatePreview(); }

void ScoreControl::addToEntry(wxTextCtrl *entry, int amount) {
  long current_score = StringUtil::stringToInt(entry->GetValue());
  entry->SetValue(StringUtil::intToString(current_score + amount));
  updatePreview();
}

void ScoreControl::homeAddOne(wxCommandEvent &event) {
  addToEntry(home_score_entry, 1);
}

void ScoreControl::homeAddFive(wxCommandEvent &event) {
  addToEntry(home_score_entry, 5);
}

void ScoreControl::homeMinusOne(wxCommandEvent &event) {
  addToEntry(home_score_entry, -1);
}

void ScoreControl::awayAddOne(wxCommandEvent &event) {
  addToEntry(away_score_entry, 1);
}

void ScoreControl::awayAddFive(wxCommandEvent &event) {
  addToEntry(away_score_entry, 5);
}

void ScoreControl::awayMinusOne(wxCommandEvent &event) {
  addToEntry(away_score_entry, -1);
}

}  // namespace cszb_scoreboard
