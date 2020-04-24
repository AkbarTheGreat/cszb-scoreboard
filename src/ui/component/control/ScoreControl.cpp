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

#include "ScoreboardCommon.h"
#include "config/TeamConfig.h"
#include "ui/UiUtil.h"
#include "ui/graphics/TeamColors.h"
#include "util/FilesystemPath.h"
#include "util/ProtoUtil.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int SCORE_FONT_SIZE = 20;
const int TEAM_FONT_SIZE = 5;
const int BORDER_SIZE = 3;
const double LOGO_OVERLAY_SCALE = 0.75;
const std::string NO_LOGO_MESSAGE = "<No Logo Selected>";

const std::string INTRO_MODE_LABEL = "Introduce Teams";
const std::string SCORE_MODE_LABEL = "Show Scores";

ScoreControl *ScoreControl::Create(PreviewPanel *preview_panel,
                                   wxWindow *parent) {
  ScoreControl *control = new ScoreControl(preview_panel, parent);
  control->initializeWidgets();
  return control;
}

void ScoreControl::createControls(wxPanel *control_panel) {
  // TODO: Populate the team names from settings-based defaults

  team_controls_panel = new wxPanel(control_panel);

  home_score_label =
      new wxStaticText(team_controls_panel, wxID_ANY, wxT("Home"));
  home_color_picker = new wxColourPickerCtrl(
      team_controls_panel, wxID_ANY,
      TeamColors::getInstance()->getColor(ProtoUtil::homeSide()));
  home_name_entry =
      new wxTextCtrl(team_controls_panel, wxID_ANY, wxT("Home Team"));
  home_score_entry = new wxTextCtrl(team_controls_panel, wxID_ANY, wxT("0"));

  home_button_panel = new wxPanel(team_controls_panel);
  home_plus_1 = new wxButton(home_button_panel, wxID_ANY, "+1",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  home_plus_5 = new wxButton(home_button_panel, wxID_ANY, "+5",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  home_minus_1 = new wxButton(home_button_panel, wxID_ANY, "-1",
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

  home_logo_label =
      new wxStaticText(team_controls_panel, wxID_ANY, NO_LOGO_MESSAGE);
  home_logo_button = new wxButton(team_controls_panel, wxID_ANY, "Choose Logo");

  away_score_label =
      new wxStaticText(team_controls_panel, wxID_ANY, wxT("Away"));
  away_color_picker = new wxColourPickerCtrl(
      team_controls_panel, wxID_ANY,
      TeamColors::getInstance()->getColor(ProtoUtil::awaySide()));

  away_name_entry =
      new wxTextCtrl(team_controls_panel, wxID_ANY, wxT("Away Team"));
  away_score_entry = new wxTextCtrl(team_controls_panel, wxID_ANY, wxT("0"));

  away_button_panel = new wxPanel(team_controls_panel);
  away_plus_1 = new wxButton(away_button_panel, wxID_ANY, "+1",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  away_plus_5 = new wxButton(away_button_panel, wxID_ANY, "+5",
                             wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  away_minus_1 = new wxButton(away_button_panel, wxID_ANY, "-1",
                              wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

  away_logo_label =
      new wxStaticText(team_controls_panel, wxID_ANY, NO_LOGO_MESSAGE);
  away_logo_button = new wxButton(team_controls_panel, wxID_ANY, "Choose Logo");

  team_intro_button =
      new wxToggleButton(control_panel, wxID_ANY, INTRO_MODE_LABEL);

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
  home_logo_button->Bind(wxEVT_BUTTON, &ScoreControl::selectLogo, this);
  team_intro_button->Bind(wxEVT_TOGGLEBUTTON, &ScoreControl::toggleIntroMode,
                          this);
}

void ScoreControl::positionWidgets(wxPanel *control_panel) {
  wxSizer *team_control_sizer = UiUtil::sizer(0, 2);

  team_control_sizer->Add(home_score_label, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(away_score_label, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(home_color_picker, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(away_color_picker, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(home_name_entry, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(away_name_entry, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(home_score_entry, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(away_score_entry, 0, wxALL, BORDER_SIZE);

  wxSizer *home_panel_sizer = UiUtil::sizer(1, 0);
  home_panel_sizer->Add(home_plus_1, 0, wxALL, BORDER_SIZE);
  home_panel_sizer->Add(home_plus_5, 0, wxALL, BORDER_SIZE);
  home_panel_sizer->Add(home_minus_1, 0, wxALL, BORDER_SIZE);
  home_button_panel->SetSizerAndFit(home_panel_sizer);
  team_control_sizer->Add(home_button_panel, 0, wxALL, BORDER_SIZE);

  wxSizer *away_panel_sizer = UiUtil::sizer(1, 0);
  away_panel_sizer->Add(away_plus_1, 0, wxALL, BORDER_SIZE);
  away_panel_sizer->Add(away_plus_5, 0, wxALL, BORDER_SIZE);
  away_panel_sizer->Add(away_minus_1, 0, wxALL, BORDER_SIZE);
  away_button_panel->SetSizerAndFit(away_panel_sizer);
  team_control_sizer->Add(away_button_panel, 0, wxALL, BORDER_SIZE);

  team_control_sizer->Add(home_logo_label, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(away_logo_label, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(home_logo_button, 0, wxALL, BORDER_SIZE);
  team_control_sizer->Add(away_logo_button, 0, wxALL, BORDER_SIZE);

  team_controls_panel->SetSizerAndFit(team_control_sizer);

  wxSizer *outer_sizer = UiUtil::sizer(0, 2);
  outer_sizer->Add(team_controls_panel, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(team_intro_button, 0, wxALL | wxALIGN_CENTRE_VERTICAL,
                   BORDER_SIZE);
  control_panel->SetSizerAndFit(outer_sizer);
}

void ScoreControl::selectLogo(wxCommandEvent &event) {
  wxFileDialog dialog(this, _("Select Logo Image"), "", "",
                      LOGO_SELECTION_STRING, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath selected_file = (std::string)dialog.GetPath();
    if (event.GetEventObject() == home_logo_button) {
      home_logo = wxImage(selected_file.c_str());
      home_logo_label->SetLabelText(selected_file.filename().c_str());
    }
    if (event.GetEventObject() == away_logo_button) {
      away_logo = wxImage(selected_file.c_str());
      away_logo_label->SetLabelText(selected_file.filename().c_str());
    }
  }
  control_panel->Update();
  updatePreview();
}

std::vector<proto::RenderableText> ScoreControl::scoreLines(bool isHome) {
  std::vector<proto::RenderableText> update;

  wxTextCtrl *score_entry = away_score_entry;
  wxTextCtrl *name_entry = away_name_entry;

  if (isHome) {
    score_entry = home_score_entry;
    name_entry = home_name_entry;
  }

  update.push_back(proto::RenderableText());
  update.back().set_text(score_entry->GetValue());
  update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  update.push_back(proto::RenderableText());
  update.back().set_text(name_entry->GetValue());
  update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  return update;
}

std::vector<proto::RenderableText> ScoreControl::introLines(bool isHome) {
  std::vector<proto::RenderableText> update;

  wxTextCtrl *name_entry = away_name_entry;
  std::string intro_text = "Visitors";

  if (isHome) {
    name_entry = home_name_entry;
    intro_text = "Home";
  }

  update.push_back(proto::RenderableText());
  update.back().set_text(name_entry->GetValue());
  update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  update.push_back(proto::RenderableText());
  update.back().set_text(intro_text);
  update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  return update;
}

void ScoreControl::updatePreview() {
  home_color_picker->SetColour(
      TeamColors::getInstance()->getColor(ProtoUtil::homeSide()));
  away_color_picker->SetColour(
      TeamColors::getInstance()->getColor(ProtoUtil::awaySide()));

  std::vector<proto::RenderableText> home_update;
  std::vector<proto::RenderableText> away_update;

  if (team_intro_button->GetValue()) {
    home_update = introLines(true);
    away_update = introLines(false);
  } else {
    home_update = scoreLines(true);
    away_update = scoreLines(false);
  }

  if (home_logo.has_value()) {
    previewPanel()->setTextForPreview(
        home_update, home_color_picker->GetColour(), true, *home_logo,
        LOGO_OVERLAY_SCALE, ProtoUtil::homeSide());
  } else {
    previewPanel()->setTextForPreview(home_update,
                                      home_color_picker->GetColour(), true,
                                      ProtoUtil::homeSide());
  }
  if (away_logo.has_value()) {
    previewPanel()->setTextForPreview(
        away_update, away_color_picker->GetColour(), true, *away_logo,
        LOGO_OVERLAY_SCALE, ProtoUtil::awaySide());
  } else {
    previewPanel()->setTextForPreview(away_update,
                                      away_color_picker->GetColour(), true,
                                      ProtoUtil::awaySide());
  }
}

void ScoreControl::homeUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::homeNameUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::awayUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::awayNameUpdated(wxKeyEvent &event) { updatePreview(); }

void ScoreControl::colorChanged(wxColourPickerEvent &event) {
  TeamColors::getInstance()->setColor(ProtoUtil::homeSide(),
                                      home_color_picker->GetColour());
  TeamColors::getInstance()->setColor(ProtoUtil::awaySide(),
                                      away_color_picker->GetColour());

  updatePreview();
}

void ScoreControl::toggleIntroMode(wxCommandEvent &event) {
  if (team_intro_button->GetValue()) {
    team_intro_button->SetLabelText(SCORE_MODE_LABEL);
  } else {
    team_intro_button->SetLabelText(INTRO_MODE_LABEL);
  }
  updatePreview();
}

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
