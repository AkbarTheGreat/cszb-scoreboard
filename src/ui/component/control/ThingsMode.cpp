/*
ui/component/control/ThingsMode.cpp: A control for managing 5/6 things in a
short-form improv show.

Copyright 2019-2021 Tracy Beck

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

#include <array>

#include "config/TeamConfig.h"
#include "ui/UiUtil.h"
#include "util/ProtoUtil.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;
const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const std::array<wxString, 2> PRESENTER_OPTIONS{
    {{"Activity List"}, {"Replacements"}}};
const int SCROLL_X_STEP = 0;
const int SCROLL_Y_STEP = 20;

auto ThingsMode::Create(PreviewPanel *preview_panel, swx::Panel *wx)
    -> std::unique_ptr<ThingsMode> {
  auto entry = std::make_unique<ThingsMode>(preview_panel, wx);
  entry->initializeWidgets();
  return entry;
}

void ThingsMode::createControls(Panel *control_panel) {
  scrollable_panel = new wxScrolledWindow(
      control_panel->wx, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);

  button_panel = std::make_unique<Panel>(new swx::Panel(scrollable_panel));

  screen_selection = std::make_unique<TeamSelector>(button_panel->childPanel());
  presenter_selection = std::make_unique<Radio>(new swx::RadioBox(
      button_panel->wx, wxID_ANY, wxT("Present"), wxDefaultPosition,
      wxDefaultSize, PRESENTER_OPTIONS.size(), PRESENTER_OPTIONS.data(), 0,
      wxRA_SPECIFY_ROWS));

  new_activity_button = button_panel->button("New Activity");
  new_replacement_button = button_panel->button("New Replacement");

  home_activities_panel =
      new ActivityPanel(scrollable_panel, this, ProtoUtil::homeSide());
  away_activities_panel =
      new ActivityPanel(scrollable_panel, this, ProtoUtil::awaySide());
  all_activities_panel =
      new ActivityPanel(scrollable_panel, this, ProtoUtil::allSide());

  positionWidgets(control_panel);
  bindEvents();

  wxSize scrollable_size = scrollable_panel->GetSize();
}

void ThingsMode::positionWidgets(Panel *control_panel) {
  // wxSizer *button_sizer = UiUtil::sizer(0, 2);
  // wxSizer *scrollable_sizer = UiUtil::sizer(0, 1);
  auto *scrollable_sizer = new wxGridBagSizer();

  button_panel->addWidget(*screen_selection, 0, 0);
  button_panel->addWidget(*presenter_selection, 0, 1);
  button_panel->addWidget(*new_activity_button, 1, 0);
  button_panel->addWidget(*new_replacement_button, 1, 1);

  button_panel->runSizer();

  UiUtil::addToGridBag(scrollable_sizer, button_panel->wx, 0, 0);
  UiUtil::addToGridBag(scrollable_sizer, home_activities_panel, 1, 0);
  UiUtil::addToGridBag(scrollable_sizer, away_activities_panel, 2, 0);
  UiUtil::addToGridBag(scrollable_sizer, all_activities_panel, 3, 0);

  updateActivityPanel();

  scrollable_panel->SetSizer(scrollable_sizer);
  scrollable_panel->FitInside();
  scrollable_panel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);

  UiUtil::addToGridBag(control_panel->sizer(), scrollable_panel, 0, 0);

  control_panel->runSizer();
}

void ThingsMode::bindEvents() {
  new_activity_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->addActivity(); });
  new_replacement_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->addReplacement(); });
  screen_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent &event) -> void { this->screenChanged(); });
  presenter_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent &event) -> void { this->presentedListChanged(); });
}

void ThingsMode::updateScreenText(ScreenText *screen_text) {
  ActivityPanel *selected_panel = all_activities_panel;
  if (screen_selection->allSelected()) {
    // Do nothing, already set
  } else if (screen_selection->homeSelected()) {
    selected_panel = home_activities_panel;
  } else if (screen_selection->awaySelected()) {
    selected_panel = away_activities_panel;
  }

  selected_panel->refreshSizers();
  Color screen_color = selected_panel->getColor();

  // Re-size for scrollable windows
  scrollable_panel->SetSizer(scrollable_panel->GetSizer());
  scrollable_panel->FitInside();
  scrollable_panel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
  scrollable_panel->SetScrollRate(SCROLL_X_STEP, SCROLL_Y_STEP);

  std::vector<proto::RenderableText> screen_lines;

  if (presenter_selection->selection() == 0) {
    screen_lines = selected_panel->previewText(DEFAULT_FONT_SIZE);
  } else {
    screen_lines =
        selected_panel->replacementPanel()->previewText(DEFAULT_FONT_SIZE);
  }

  screen_text->setAllText(screen_lines, screen_color, true,
                          ProtoUtil::allSide());
}

void ThingsMode::textUpdated() { updatePreview(); }

void ThingsMode::updateActivityPanel() {
  if (screen_selection->allSelected()) {
    home_activities_panel->Hide();
    away_activities_panel->Hide();
    all_activities_panel->Show();
  } else if (screen_selection->homeSelected()) {
    away_activities_panel->Hide();
    all_activities_panel->Hide();
    home_activities_panel->Show();
  } else if (screen_selection->awaySelected()) {
    home_activities_panel->Hide();
    all_activities_panel->Hide();
    away_activities_panel->Show();
  }
}

void ThingsMode::screenChanged() {
  updateActivityPanel();
  updatePreview();
}

void ThingsMode::presentedListChanged() { updatePreview(); }

void ThingsMode::addActivity() {
  if (screen_selection->allSelected()) {
    all_activities_panel->addActivity(scrollable_panel);
  } else if (screen_selection->homeSelected()) {
    home_activities_panel->addActivity(scrollable_panel);
  } else if (screen_selection->awaySelected()) {
    away_activities_panel->addActivity(scrollable_panel);
  }
}

void ThingsMode::addReplacement() {
  if (screen_selection->allSelected()) {
    all_activities_panel->addReplacement();
  } else if (screen_selection->homeSelected()) {
    home_activities_panel->addReplacement();
  } else if (screen_selection->awaySelected()) {
    away_activities_panel->addReplacement();
  }
}

}  // namespace cszb_scoreboard
