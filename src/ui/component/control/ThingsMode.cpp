/*
ui/component/control/ThingsMode.cpp: A control for managing 5/6 things in a
short-form improv show.

Copyright 2019-2026 Tracy Beck

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

#include "ui/component/control/ThingsMode.h"

#include <array>   // for array
#include <vector>  // for vector

#include "ScoreboardCommon.h"                                // for DEFAULT_...
#include "config.pb.h"                                       // for Renderab...
#include "config/swx/event.h"                                // for wxEVT_CO...
#include "ui/component/ScreenText.h"                         // for ScreenText
#include "ui/component/control/things_mode/ActivityPanel.h"  // for Activity...
#include "ui/component/control/things_mode/HeadToHeadActivityPanel.h"  // for Hea...
#include "ui/graphics/Color.h"  // for Color
#include "util/ProtoUtil.h"     // for ProtoUtil

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;
const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

constexpr int PRESENT_ACTIVITY_LIST = 0;
constexpr int PRESENT_REPLACEMENTS = 1;
static constexpr std::array<const char*, 2> PRESENTER_OPTIONS{
    {"Activity List", "Replacements"}};

constexpr int SINGLE_TEAM_MODE = 0;
constexpr int HEAD_TO_HEAD_MODE = 1;
static constexpr std::array<const char*, 2> GAME_MODE_OPTIONS{
    {"Single Team", "Head-to-Head"}};

auto ThingsMode::Create(swx::Panel* wx) -> std::unique_ptr<ThingsMode> {
  auto entry = std::make_unique<ThingsMode>(wx);
  entry->initializeWidgets();
  return entry;
}

void ThingsMode::createControls(Panel* control_panel) {
  scrollable_panel = control_panel->scrollingPanel();

  button_panel = scrollable_panel->panel();

  screen_selection = std::make_unique<TeamSelector>(button_panel->childPanel());
  presenter_selection = button_panel->radio("Present", PRESENTER_OPTIONS.data(),
                                            PRESENTER_OPTIONS.size());

  game_mode_selection = button_panel->radio(
      "Game Mode", GAME_MODE_OPTIONS.data(), GAME_MODE_OPTIONS.size());

  new_activity_button = button_panel->button("New Activity");
  new_replacement_button = button_panel->button("New Replacement");

  activity_panels =
      std::make_unique<MultiWidgetPanel>(scrollable_panel->childPanel());
  home_activities_panel = std::make_unique<SingleTeamActivityPanel>(
      activity_panels->childPanel(), this, ProtoUtil::homeSide());
  away_activities_panel = std::make_unique<SingleTeamActivityPanel>(
      activity_panels->childPanel(), this, ProtoUtil::awaySide());
  all_activities_panel = std::make_unique<SingleTeamActivityPanel>(
      activity_panels->childPanel(), this, ProtoUtil::allSide());
  head_to_head_activities_panel = std::make_unique<HeadToHeadActivityPanel>(
      activity_panels->childPanel(), this);

  positionWidgets(control_panel);
  bindEvents();
}

void ThingsMode::positionWidgets(Panel* control_panel) {
  button_panel->addWidget(*screen_selection, 0, 0);
  button_panel->addWidget(*presenter_selection, 0, 1);
  button_panel->addWidget(*game_mode_selection, 0, 2);
  button_panel->addWidget(*new_activity_button, 1, 0);
  button_panel->addWidget(*new_replacement_button, 1, 1);

  button_panel->runSizer();

  scrollable_panel->addWidget(*button_panel, 0, 0);
  scrollable_panel->addWidget(*activity_panels, 1, 0);

  activity_panels->addWidget(*home_activities_panel);
  activity_panels->addWidget(*away_activities_panel);
  activity_panels->addWidget(*all_activities_panel);
  activity_panels->addWidget(*head_to_head_activities_panel);

  updateActivityPanel();

  scrollable_panel->runSizer();

  control_panel->addWidget(*scrollable_panel, 0, 0);

  control_panel->runSizer();
}

void ThingsMode::bindEvents() {
  new_activity_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent& event) -> void { this->addActivity(); });
  new_replacement_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent& event) -> void { this->addReplacement(); });
  screen_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent& event) -> void { this->screenChanged(); });
  presenter_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent& event) -> void { this->presentedListChanged(); });
  game_mode_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent& event) -> void { this->gameModeChanged(); });
}

void ThingsMode::updateScreenText(ScreenText* screen_text) {
  if (!isActive()) {
    return;
  }

  ActivityPanel* selected_panel = head_to_head_activities_panel.get();

  if (game_mode_selection->selection() == SINGLE_TEAM_MODE) {
    if (screen_selection->allSelected()) {
      selected_panel = all_activities_panel.get();
    } else if (screen_selection->homeSelected()) {
      selected_panel = home_activities_panel.get();
    } else if (screen_selection->awaySelected()) {
      selected_panel = away_activities_panel.get();
    }
  }

  selected_panel->refreshSizers();

  // Re-size for scrollable windows
  scrollable_panel->runSizer();

  std::vector<proto::RenderableText> home_screen_lines, away_screen_lines;
  bool presenting_replacement =
      presenter_selection->selection() == PRESENT_REPLACEMENTS;

  Color home_screen_color =
      selected_panel->color(ProtoUtil::homeSide(), presenting_replacement);
  Color away_screen_color =
      selected_panel->color(ProtoUtil::awaySide(), presenting_replacement);

  if (presenting_replacement) {
    home_screen_lines = selected_panel->replacementText(ProtoUtil::homeSide(),
                                                        DEFAULT_FONT_SIZE);
    away_screen_lines = selected_panel->replacementText(ProtoUtil::awaySide(),
                                                        DEFAULT_FONT_SIZE);
  } else {
    home_screen_lines =
        selected_panel->activityText(ProtoUtil::homeSide(), DEFAULT_FONT_SIZE);
    away_screen_lines =
        selected_panel->activityText(ProtoUtil::awaySide(), DEFAULT_FONT_SIZE);
  }

  if (selected_panel->splitScreens(presenting_replacement)) {
    screen_text->setAllText(home_screen_lines, home_screen_color, true,
                            ProtoUtil::homeSide());
    screen_text->setAllText(away_screen_lines, away_screen_color, true,
                            ProtoUtil::awaySide());
  } else {
    screen_text->setAllText(home_screen_lines, home_screen_color, true,
                            ProtoUtil::allSide());
  }
  return;
}

void ThingsMode::textUpdated() { updatePreview(); }

void ThingsMode::updateActivityPanel() {
  if (game_mode_selection->selection() == HEAD_TO_HEAD_MODE) {
    activity_panels->showWidget(*head_to_head_activities_panel);
  } else {
    if (screen_selection->allSelected()) {
      activity_panels->showWidget(*all_activities_panel);
    } else if (screen_selection->homeSelected()) {
      activity_panels->showWidget(*home_activities_panel);
    } else if (screen_selection->awaySelected()) {
      activity_panels->showWidget(*away_activities_panel);
    }
  }
}

void ThingsMode::screenChanged() {
  updateActivityPanel();
  updatePreview();
}

void ThingsMode::presentedListChanged() { updatePreview(); }

void ThingsMode::gameModeChanged() {
  if (game_mode_selection->selection() == 0) {
    // Single team mode, enable screen selection, use the selected team's
    // lists.
    screen_selection->enable();
  } else {
    // Head-to-head mode, disable screen selection and use the head-to-head
    // configurations.
    screen_selection->disable();
  }
  screenChanged();
}

void ThingsMode::addActivity() {
  if (game_mode_selection->selection() == HEAD_TO_HEAD_MODE) {
    head_to_head_activities_panel->addActivity();
    return;
  }

  if (screen_selection->allSelected()) {
    all_activities_panel->addActivity();
  } else if (screen_selection->homeSelected()) {
    home_activities_panel->addActivity();
  } else if (screen_selection->awaySelected()) {
    away_activities_panel->addActivity();
  }
}

void ThingsMode::addReplacement() {
  if (game_mode_selection->selection() == HEAD_TO_HEAD_MODE) {
    head_to_head_activities_panel->addReplacement();
    return;
  }

  if (screen_selection->allSelected()) {
    all_activities_panel->addReplacement();
  } else if (screen_selection->homeSelected()) {
    home_activities_panel->addReplacement();
  } else if (screen_selection->awaySelected()) {
    away_activities_panel->addReplacement();
  }
}

}  // namespace cszb_scoreboard
