/*
ui/component/control/ScoreControl.cpp: This class is responsible for generating
a score update which is sent to all scoreboard screens.

Copyright 2019-2025 Tracy Beck

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

#include <algorithm>   // for max
#include <cstdint>     // for int64_t
#include <filesystem>  // for path
#include <string>      // for string

#include "ScoreboardCommon.h"             // for LOGO_SELECTION_STRING
#include "config.pb.h"                    // for RenderableText, Font, Rende...
#include "config/TeamConfig.h"            // for TeamConfig
#include "config/swx/defs.h"              // for operator|, wxAlignment, wxD...
#include "config/swx/event.h"             // for wxEVT_COMMAND_BUTTON_CLICKED
#include "team_library.pb.h"              // for TeamLibraryDialogResponse
#include "ui/component/ScreenText.h"      // for ScreenText
#include "ui/component/ScreenTextSide.h"  // for OverlayScreenPosition
#include "ui/dialog/TeamLibraryDialog.h"  // for TeamLibraryDialog
#include "ui/frame/FrameManager.h"        // for FrameManager
#include "ui/frame/HotkeyTable.h"         // for HotkeyTable, wxAcceleratorE...
#include "ui/frame/MainView.h"            // for MainView
#include "ui/graphics/Color.h"            // for Color
#include "ui/graphics/TeamColors.h"       // for TeamColors
#include "ui/widget/FilePicker.h"         // for FilePicker
#include "util/FilesystemPath.h"          // for FilesystemPath
#include "util/ProtoUtil.h"               // for ProtoUtil
#include "util/Singleton.h"               // for Singleton
#include "util/StringUtil.h"              // for StringUtil

namespace cszb_scoreboard {
class Widget;

const int SCORE_FONT_SIZE = 20;
const int TEAM_FONT_SIZE = 5;
const int BORDER_SIZE = 3;
const double LOGO_OVERLAY_SCALE = 0.65;
const unsigned char LOGO_ALPHA = 96;

const std::string NO_LOGO_MESSAGE = "<No Logo Selected>";

const std::string INTRO_MODE_LABEL = "Introduce Teams";
const std::string SCORE_MODE_LABEL = "Show Scores";

auto ScoreControl::Create(swx::Panel *wx) -> std::unique_ptr<ScoreControl> {
  auto control = std::make_unique<ScoreControl>(wx);
  control->initializeWidgets();
  return control;
}

void ScoreControl::onLibraryDialogClose() {
  library_dialog.reset();
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog.
  focus();
}

void ScoreControl::setTeams(const proto::TeamLibraryDialogResponse &teams) {
  if (teams.has_home()) {
    home_name_entry->setValue(teams.home().name());
    home_logo_label->set(teams.home().image_path());
    if (!teams.home().image_path().empty()) {
      home_logo = Image(FilesystemPath(teams.home().image_path()));
    }
  }
  if (teams.has_away()) {
    away_name_entry->setValue(teams.away().name());
    away_logo_label->set(teams.away().image_path());
    if (!teams.away().image_path().empty()) {
      away_logo = Image(FilesystemPath(teams.away().image_path()));
    }
  }
  control_panel->update();
  updatePreview();
}

void ScoreControl::createControls(Panel *control_panel) {
  // TODO(akbar): Populate the team names from settings-based defaults

  team_controls_panel = control_panel->panel();

  home_score_label = team_controls_panel->label("Home");
  home_color_picker = team_controls_panel->colorPicker(
      singleton->teamColors()->getColor(ProtoUtil::homeSide()));
  home_name_entry = team_controls_panel->text("Home Team");
  home_score_entry = team_controls_panel->text("0");

  home_button_panel = team_controls_panel->panel();
  home_plus_1 = home_button_panel->button("+1", true);
  home_plus_5 = home_button_panel->button("+5", true);
  home_minus_1 = home_button_panel->button("-1", true);

  home_logo_label = team_controls_panel->label(NO_LOGO_MESSAGE);
  home_logo_button = team_controls_panel->button("Choose Logo");

  away_score_label = team_controls_panel->label("Away");
  away_color_picker = team_controls_panel->colorPicker(
      singleton->teamColors()->getColor(ProtoUtil::awaySide()));
  away_name_entry = team_controls_panel->text("Away Team");
  away_score_entry = team_controls_panel->text("0");

  away_button_panel = team_controls_panel->panel();
  away_plus_1 = away_button_panel->button("+1", true);
  away_plus_5 = away_button_panel->button("+5", true);
  away_minus_1 = away_button_panel->button("-1", true);

  away_logo_label = team_controls_panel->label(NO_LOGO_MESSAGE);
  away_logo_button = team_controls_panel->button("Choose Logo");

  right_panel = control_panel->panel();
  team_intro_button = right_panel->toggle(INTRO_MODE_LABEL);
  team_library_button = right_panel->button("Team Library");

  positionWidgets(control_panel);
  bindEvents();
}

void ScoreControl::bindEvents() {
  home_score_entry->bind(
      wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void { this->homeUpdated(); });
  home_name_entry->bind(wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void {
    this->homeNameUpdated();
  });
  away_score_entry->bind(
      wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void { this->awayUpdated(); });
  away_name_entry->bind(wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void {
    this->awayNameUpdated();
  });
  home_plus_1->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->homeAddOne(); });
  home_plus_5->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->homeAddFive(); });
  home_minus_1->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->homeMinusOne(); });
  away_plus_1->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->awayAddOne(); });
  away_plus_5->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->awayAddFive(); });
  away_minus_1->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->awayMinusOne(); });
  home_color_picker->bind(
      wxEVT_COLOURPICKER_CHANGED,
      [this](wxColourPickerEvent &event) -> void { this->colorChanged(); });
  away_color_picker->bind(
      wxEVT_COLOURPICKER_CHANGED,
      [this](wxColourPickerEvent &event) -> void { this->colorChanged(); });
  home_logo_button->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->selectLogo(true);
  });
  away_logo_button->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->selectLogo(false);
  });
  team_intro_button->bind(
      wxEVT_TOGGLEBUTTON,
      [this](wxCommandEvent &event) -> void { this->toggleIntroMode(); });
  team_library_button->bind(
      wxEVT_BUTTON,
      [this](wxCommandEvent &event) -> void { this->selectFromLibrary(); });
}

/* Since the control panel alternates between home and away buttons to make
 * columns, this method adds them in the correct order to mirror the
 * single-window positioning. */
void ScoreControl::addHomeAwayWidgetPair(Panel *panel, int row,
                                         const Widget &home_widget,
                                         const Widget &away_widget) {
  bool is_left = true;
  int col = 0;
  for (auto team : singleton->teamConfig()->singleScreenOrder()) {
    Button *minus_1 = home_minus_1.get();
    Button *plus_1 = home_plus_1.get();
    Button *plus_5 = home_plus_5.get();

    if (team == proto::TeamInfo_TeamType_HOME_TEAM) {
      panel->addWidget(home_widget, row, col++, BORDER_SIZE);
      minus_1 = home_minus_1.get();
      plus_1 = home_plus_1.get();
      plus_5 = home_plus_5.get();
    } else if (team == proto::TeamInfo_TeamType_AWAY_TEAM) {
      panel->addWidget(away_widget, row, col++, BORDER_SIZE);
      minus_1 = away_minus_1.get();
      plus_1 = away_plus_1.get();
      plus_5 = away_plus_5.get();
    }

    // Bind the hotkeys the first pass through, then skip it on subsequent runs.
    if (row == 0) {
      if (is_left) {
        singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, 'z', minus_1->id());
        minus_1->toolTip("Ctrl+z");
        singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, 'a', plus_1->id());
        plus_1->toolTip("Ctrl+a");
        singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, 'q', plus_5->id());
        plus_5->toolTip("Ctrl+q");
        is_left = false;
      } else {
        singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, 'x', minus_1->id());
        minus_1->toolTip("Ctrl+x");
        singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, 's', plus_1->id());
        plus_1->toolTip("Ctrl+s");
        singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, 'w', plus_5->id());
        plus_5->toolTip("Ctrl+w");
      }
    }
  }
}

void ScoreControl::positionWidgets(Panel *control_panel) {
  int row = 0;
  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_score_label,
                        *away_score_label);
  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_color_picker,
                        *away_color_picker);
  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_name_entry,
                        *away_name_entry);
  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_score_entry,
                        *away_score_entry);

  int col = 0;
  home_button_panel->addWidget(*home_plus_5, 0, col++, BORDER_SIZE);
  home_button_panel->addWidget(*home_plus_1, 0, col++, BORDER_SIZE);
  home_button_panel->addWidget(*home_minus_1, 0, col++, BORDER_SIZE);

  col = 0;
  away_button_panel->addWidget(*away_plus_5, 0, col++, BORDER_SIZE);
  away_button_panel->addWidget(*away_plus_1, 0, col++, BORDER_SIZE);
  away_button_panel->addWidget(*away_minus_1, 0, col++, BORDER_SIZE);

  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_button_panel,
                        *away_button_panel);
  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_logo_label,
                        *away_logo_label);
  addHomeAwayWidgetPair(team_controls_panel.get(), row++, *home_logo_button,
                        *away_logo_button);

  right_panel->addWidget(*team_intro_button, 0, 0);
  right_panel->addWidget(*team_library_button, 1, 0);

  col = 0;
  control_panel->addWidget(*team_controls_panel, 0, col++, BORDER_SIZE);
  control_panel->addWidget(*right_panel, 0, col++, BORDER_SIZE,
                           wxALL | wxALIGN_CENTER_VERTICAL);

  home_button_panel->runSizer();
  away_button_panel->runSizer();
  team_controls_panel->runSizer();
  control_panel->runSizer();
}

void ScoreControl::selectLogo(bool isHome) {
  std::unique_ptr<FilePicker> picker =
      openFilePicker("Select Logo Image", LOGO_SELECTION_STRING);
  std::optional<FilesystemPath> selected_file = picker->selectFile();
  if (selected_file.has_value()) {
    if (isHome) {
      home_logo = Image(*selected_file);
      home_logo_label->set(selected_file->filename().string());
    } else {
      away_logo = Image(*selected_file);
      away_logo_label->set(selected_file->filename().string());
    }
  }
  control_panel->update();
  updatePreview();
}

auto ScoreControl::scoreLines(bool isHome)
    -> std::vector<proto::RenderableText> {
  std::vector<proto::RenderableText> update;

  Text *score_entry = away_score_entry.get();
  Text *name_entry = away_name_entry.get();

  if (isHome) {
    score_entry = home_score_entry.get();
    name_entry = home_name_entry.get();
  }

  update.emplace_back();
  update.back().set_text(score_entry->value());
  update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  update.emplace_back();
  update.back().set_text(name_entry->value());
  update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  return update;
}

auto ScoreControl::introLines(bool isHome)
    -> std::vector<proto::RenderableText> {
  std::vector<proto::RenderableText> update;

  Text *name_entry = away_name_entry.get();
  std::string intro_text = "Visitors";

  if (isHome) {
    name_entry = home_name_entry.get();
    intro_text = "Home";
  }

  update.emplace_back();
  update.back().set_text(name_entry->value());
  update.back().mutable_font()->set_size(SCORE_FONT_SIZE);
  update.emplace_back();
  update.back().set_text(intro_text);
  update.back().mutable_font()->set_size(TEAM_FONT_SIZE);
  update.back().set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP);

  return update;
}

void ScoreControl::updateScreenText(ScreenText *screen_text) {
  if (isActive()) {
    updateScreenText(screen_text, team_intro_button->value());
  }

  FrameManager *frameMgr = singleton->frameManager();
  if (frameMgr != nullptr) {
    MainView *main = frameMgr->mainView();
    if (main != nullptr) {
      ScreenText *quick_score = main->scoreQuickState();
      if (quick_score != nullptr) {
        updateScreenText(quick_score, false);
      }
    }
  }
}

void ScoreControl::updateScreenText(ScreenText *screen_text, bool team_intro) {
  home_color_picker->setColor(
      singleton->teamColors()->getColor(ProtoUtil::homeSide()));
  away_color_picker->setColor(
      singleton->teamColors()->getColor(ProtoUtil::awaySide()));

  std::vector<proto::RenderableText> home_update;
  std::vector<proto::RenderableText> away_update;

  OverlayScreenPosition logo_position;

  if (team_intro) {
    home_update = introLines(true);
    away_update = introLines(false);
    logo_position = OverlayScreenPosition::Centered;
  } else {
    home_update = scoreLines(true);
    away_update = scoreLines(false);
    logo_position = OverlayScreenPosition::BottomLeft;
  }

  if (home_logo.has_value()) {
    screen_text->setAllText(home_update, home_color_picker->color(), true,
                            *home_logo, LOGO_OVERLAY_SCALE, LOGO_ALPHA,
                            logo_position, ProtoUtil::homeSide());
  } else {
    screen_text->setAllText(home_update, home_color_picker->color(), true,
                            ProtoUtil::homeSide());
  }
  if (away_logo.has_value()) {
    screen_text->setAllText(away_update, away_color_picker->color(), true,
                            *away_logo, LOGO_OVERLAY_SCALE, LOGO_ALPHA,
                            logo_position, ProtoUtil::awaySide());
  } else {
    screen_text->setAllText(away_update, away_color_picker->color(), true,
                            ProtoUtil::awaySide());
  }
}

void ScoreControl::homeUpdated() { updatePreview(); }

void ScoreControl::homeNameUpdated() { updatePreview(); }

void ScoreControl::awayUpdated() { updatePreview(); }

void ScoreControl::awayNameUpdated() { updatePreview(); }

void ScoreControl::colorChanged() {
  singleton->teamColors()->setColor(ProtoUtil::homeSide(),
                                    home_color_picker->color());
  singleton->teamColors()->setColor(ProtoUtil::awaySide(),
                                    away_color_picker->color());

  updatePreview();
}

void ScoreControl::toggleIntroMode() {
  if (team_intro_button->value()) {
    team_intro_button->setLabel(SCORE_MODE_LABEL);
  } else {
    team_intro_button->setLabel(INTRO_MODE_LABEL);
  }
  updatePreview();
}

void ScoreControl::selectFromLibrary() {
  library_dialog =
      std::make_unique<TeamLibraryDialog>(childDialog("Team Library"), this);
  library_dialog->show();
}

void ScoreControl::addToEntry(Text *entry, int amount) {
  int64_t current_score = StringUtil::stringToInt(entry->value());
  entry->setValue(current_score + amount);
  updatePreview();
}

void ScoreControl::homeAddOne() { addToEntry(home_score_entry.get(), 1); }

void ScoreControl::homeAddFive() {
  addToEntry(home_score_entry.get(),
             5);  // NOLINT(readability-magic-numbers) Nothing
                  // else could make sense in "Add Five."
}

void ScoreControl::homeMinusOne() { addToEntry(home_score_entry.get(), -1); }

void ScoreControl::awayAddOne() { addToEntry(away_score_entry.get(), 1); }

void ScoreControl::awayAddFive() {
  addToEntry(away_score_entry.get(),
             5);  // NOLINT(readability-magic-numbers) Nothing
                  // else could make sense in "Add Five."
}

void ScoreControl::awayMinusOne() { addToEntry(away_score_entry.get(), -1); }

}  // namespace cszb_scoreboard
