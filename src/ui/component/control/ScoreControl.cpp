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
#include "ui/frame/HotkeyTable.h"
#include "ui/graphics/TeamColors.h"
#include "util/FilesystemPath.h"
#include "util/ProtoUtil.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int SCORE_FONT_SIZE = 20;
const int TEAM_FONT_SIZE = 5;
const int BORDER_SIZE = 3;
const double LOGO_OVERLAY_SCALE = 0.65;
const unsigned char LOGO_ALPHA = 96;

const std::string NO_LOGO_MESSAGE = "<No Logo Selected>";

const std::string INTRO_MODE_LABEL = "Introduce Teams";
const std::string SCORE_MODE_LABEL = "Show Scores";

auto ScoreControl::Create(PreviewPanel* preview_panel, wxWindow* parent)
    -> ScoreControl* {
  auto* control = new ScoreControl(preview_panel, parent);
  control->initializeWidgets();
  return control;
}

void ScoreControl::createControls(wxPanel* control_panel) {
  // TODO(akbar): Populate the team names from settings-based defaults

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
  away_logo_button->Bind(wxEVT_BUTTON, &ScoreControl::selectLogo, this);
  team_intro_button->Bind(wxEVT_TOGGLEBUTTON, &ScoreControl::toggleIntroMode,
                          this);
}

/* Since the conrol panel alternates between home and away buttons to make
 * columns, this method adds them in the correct order to mirror the
 * single-window positioning. */
void ScoreControl::addHomeAwayWidgetPair(wxSizer* sizer, wxWindow* home_widget,
                                         wxWindow* away_widget) {
  bool is_left = true;
  for (auto team : TeamConfig::getInstance()->singleScreenOrder()) {
    wxWindow* minus_1 = home_minus_1;
    wxWindow* plus_1 = home_plus_1;
    wxWindow* plus_5 = home_plus_5;

    if (team == proto::TeamInfo_TeamType_HOME_TEAM) {
      sizer->Add(home_widget, 0, wxALL, BORDER_SIZE);
      minus_1 = home_minus_1;
      plus_1 = home_plus_1;
      plus_5 = home_plus_5;
    } else if (team == proto::TeamInfo_TeamType_AWAY_TEAM) {
      sizer->Add(away_widget, 0, wxALL, BORDER_SIZE);
      minus_1 = away_minus_1;
      plus_1 = away_plus_1;
      plus_5 = away_plus_5;
    }

    if (is_left) {
      HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, 'z',
                                            minus_1->GetId());
      minus_1->SetToolTip("Ctrl+z");
      HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, 'a', plus_1->GetId());
      plus_1->SetToolTip("Ctrl+a");
      HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, 'q', plus_5->GetId());
      plus_5->SetToolTip("Ctrl+q");
      is_left = false;
    } else {
      HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, 'x',
                                            minus_1->GetId());
      minus_1->SetToolTip("Ctrl+x");
      HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, 's', plus_1->GetId());
      plus_1->SetToolTip("Ctrl+s");
      HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, 'w', plus_5->GetId());
      plus_5->SetToolTip("Ctrl+w");
    }
  }
}

void ScoreControl::positionWidgets(wxPanel* control_panel) {
  wxSizer* team_control_sizer = UiUtil::sizer(0, 2);

  addHomeAwayWidgetPair(team_control_sizer, home_score_label, away_score_label);
  addHomeAwayWidgetPair(team_control_sizer, home_color_picker,
                        away_color_picker);
  addHomeAwayWidgetPair(team_control_sizer, home_name_entry, away_name_entry);
  addHomeAwayWidgetPair(team_control_sizer, home_score_entry, away_score_entry);

  wxSizer* home_panel_sizer = UiUtil::sizer(1, 0);
  home_panel_sizer->Add(home_plus_5, 0, wxALL, BORDER_SIZE);
  home_panel_sizer->Add(home_plus_1, 0, wxALL, BORDER_SIZE);
  home_panel_sizer->Add(home_minus_1, 0, wxALL, BORDER_SIZE);
  home_button_panel->SetSizerAndFit(home_panel_sizer);

  wxSizer* away_panel_sizer = UiUtil::sizer(1, 0);
  away_panel_sizer->Add(away_plus_5, 0, wxALL, BORDER_SIZE);
  away_panel_sizer->Add(away_plus_1, 0, wxALL, BORDER_SIZE);
  away_panel_sizer->Add(away_minus_1, 0, wxALL, BORDER_SIZE);
  away_button_panel->SetSizerAndFit(away_panel_sizer);

  addHomeAwayWidgetPair(team_control_sizer, home_button_panel,
                        away_button_panel);
  addHomeAwayWidgetPair(team_control_sizer, home_logo_label, away_logo_label);
  addHomeAwayWidgetPair(team_control_sizer, home_logo_button, away_logo_button);

  team_controls_panel->SetSizerAndFit(team_control_sizer);

  wxSizer* outer_sizer = UiUtil::sizer(0, 2);
  outer_sizer->Add(team_controls_panel, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(team_intro_button, 0, wxALL | wxALIGN_CENTRE_VERTICAL,
                   BORDER_SIZE);
  control_panel->SetSizerAndFit(outer_sizer);
}

void ScoreControl::selectLogo(wxCommandEvent& event) {
  wxFileDialog dialog(this, _("Select Logo Image"), "", "",
                      LOGO_SELECTION_STRING, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath selected_file =
        FilesystemPath(std::string(dialog.GetPath()));
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

auto ScoreControl::scoreLines(bool isHome)
    -> std::vector<proto::RenderableText> {
  std::vector<proto::RenderableText> update;

  wxTextCtrl* score_entry = away_score_entry;
  wxTextCtrl* name_entry = away_name_entry;

  if (isHome) {
    score_entry = home_score_entry;
    name_entry = home_name_entry;
  }

  update.emplace_back(proto::RenderableText());
  update.back().set_text(score_entry->GetValue());
  update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  update.emplace_back(proto::RenderableText());
  update.back().set_text(name_entry->GetValue());
  update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  return update;
}

auto ScoreControl::introLines(bool isHome)
    -> std::vector<proto::RenderableText> {
  std::vector<proto::RenderableText> update;

  wxTextCtrl* name_entry = away_name_entry;
  std::string intro_text = "Visitors";

  if (isHome) {
    name_entry = home_name_entry;
    intro_text = "Home";
  }

  update.emplace_back(proto::RenderableText());
  update.back().set_text(name_entry->GetValue());
  update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  update.emplace_back(proto::RenderableText());
  update.back().set_text(intro_text);
  update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  return update;
}

void ScoreControl::updateScreenText(ScreenText* screen_text) {
  home_color_picker->SetColour(
      TeamColors::getInstance()->getColor(ProtoUtil::homeSide()));
  away_color_picker->SetColour(
      TeamColors::getInstance()->getColor(ProtoUtil::awaySide()));

  std::vector<proto::RenderableText> home_update;
  std::vector<proto::RenderableText> away_update;

  OverlayScreenPosition logo_position;

  if (team_intro_button->GetValue()) {
    home_update = introLines(true);
    away_update = introLines(false);
    logo_position = OverlayScreenPosition::Centered;
  } else {
    home_update = scoreLines(true);
    away_update = scoreLines(false);
    logo_position = OverlayScreenPosition::BottomLeft;
  }

  if (home_logo.has_value()) {
    screen_text->setAllText(home_update, Color(home_color_picker->GetColour()),
                            true, *home_logo, LOGO_OVERLAY_SCALE, LOGO_ALPHA,
                            logo_position, ProtoUtil::homeSide());
  } else {
    screen_text->setAllText(home_update, Color(home_color_picker->GetColour()),
                            true, ProtoUtil::homeSide());
  }
  if (away_logo.has_value()) {
    screen_text->setAllText(away_update, Color(away_color_picker->GetColour()),
                            true, *away_logo, LOGO_OVERLAY_SCALE, LOGO_ALPHA,
                            logo_position, ProtoUtil::awaySide());
  } else {
    screen_text->setAllText(away_update, Color(away_color_picker->GetColour()),
                            true, ProtoUtil::awaySide());
  }
}

void ScoreControl::homeUpdated(wxKeyEvent& event) { updatePreview(); }

void ScoreControl::homeNameUpdated(wxKeyEvent& event) { updatePreview(); }

void ScoreControl::awayUpdated(wxKeyEvent& event) { updatePreview(); }

void ScoreControl::awayNameUpdated(wxKeyEvent& event) { updatePreview(); }

void ScoreControl::colorChanged(wxColourPickerEvent& event) {
  TeamColors::getInstance()->setColor(ProtoUtil::homeSide(),
                                      Color(home_color_picker->GetColour()));
  TeamColors::getInstance()->setColor(ProtoUtil::awaySide(),
                                      Color(away_color_picker->GetColour()));

  updatePreview();
}

void ScoreControl::toggleIntroMode(wxCommandEvent& event) {
  if (team_intro_button->GetValue()) {
    team_intro_button->SetLabelText(SCORE_MODE_LABEL);
  } else {
    team_intro_button->SetLabelText(INTRO_MODE_LABEL);
  }
  updatePreview();
}

void ScoreControl::addToEntry(wxTextCtrl* entry, int amount) {
  int32_t current_score = StringUtil::stringToInt(entry->GetValue());
  entry->SetValue(StringUtil::intToString(current_score + amount));
  updatePreview();
}

void ScoreControl::homeAddOne(wxCommandEvent& event) {
  addToEntry(home_score_entry, 1);
}

void ScoreControl::homeAddFive(wxCommandEvent& event) {
  addToEntry(home_score_entry, 5);  // NOLINT(readability-magic-numbers) Nothing
                                    // else could make sense in "Add Five."
}

void ScoreControl::homeMinusOne(wxCommandEvent& event) {
  addToEntry(home_score_entry, -1);
}

void ScoreControl::awayAddOne(wxCommandEvent& event) {
  addToEntry(away_score_entry, 1);
}

void ScoreControl::awayAddFive(wxCommandEvent& event) {
  addToEntry(away_score_entry, 5);  // NOLINT(readability-magic-numbers) Nothing
                                    // else could make sense in "Add Five."
}

void ScoreControl::awayMinusOne(wxCommandEvent& event) {
  addToEntry(away_score_entry, -1);
}

}  // namespace cszb_scoreboard
