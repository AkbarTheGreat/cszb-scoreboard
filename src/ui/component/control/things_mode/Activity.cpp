/*
ui/component/control/things_mode/Activity.cpp: Represents an activity in 5/6
things.

Copyright 2019-2023 Tracy Beck

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

#include "ui/component/control/things_mode/Activity.h"

#include <utility>

#include "ScoreboardCommon.h"
#include "config/swx/event.h"
#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

Activity::Activity(ActivityPanel *parent, Panel *activity_frame,
                   Panel *replacement_frame, int index, bool is_first) {
  this->index = index;
  this->parent = parent;

  control_pane = activity_frame->panel(true);

  activity_selector = control_pane->radioButton();

  if (is_first) {
    activity_selector->setSelected(true);
  }
  activity_text = control_pane->text("", true);
  up_button = control_pane->button("^", true);
  down_button = control_pane->button("v", true);
  remove_activity_button = control_pane->button("X", true);
  replacement_panel = std::make_unique<ReplacementPanel>(
      replacement_frame->childPanel(), parent);
  bindEvents();
  positionWidgets();
}

void Activity::copyFrom(Activity *other) {
  activity_selector->setSelected(other->activity_selector->selected());
  activity_text->setValue(other->activity_text->value());
  replacement_panel = std::move(other->replacement_panel);
}

void Activity::bindEvents() {
  auto *ap = parent;
  activity_selector->bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                          [this, ap](wxCommandEvent &event) -> void {
                            ap->selectionChanged(this);
                          });

  up_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->moveButton(true); });
  down_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->moveButton(false); });

  remove_activity_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this, ap](wxCommandEvent &event) -> void { ap->deleteActivity(this); });
  activity_text->bind(wxEVT_KEY_UP,
                      [ap](wxKeyEvent &event) -> void { ap->textUpdated(); });
}

void Activity::positionWidgets() {
  control_pane->addWidget(*activity_selector, 0, 0);
  control_pane->addWidgetWithSpan(*activity_text, 0, 1, 2, 1);
  control_pane->addWidget(*up_button, 0, 2);
  control_pane->addWidget(*down_button, 0, 3);
  control_pane->addWidget(*remove_activity_button, 0, 4);
  control_pane->runSizer();
}

void Activity::moveButton(bool is_up) {
  if (is_up) {
    parent->swapActivities(index, index - 1);
  } else {
    parent->swapActivities(index, index + 1);
  }
}

void Activity::select() {
  if (!activity_selector->selected()) {
    activity_selector->setSelected(true);
    parent->selectionChanged(this);
  }
}

void Activity::unselect() { activity_selector->setSelected(false); }

auto Activity::isSelected() -> bool { return activity_selector->selected(); }

void Activity::setIndex(int index, int max_index) {
  this->index = index;
  if (index == 0) {
    up_button->disable();
  } else {
    up_button->enable();
  }
  if (index == max_index) {
    down_button->disable();
  } else {
    down_button->enable();
  }
}

auto Activity::previewText() -> std::string {
  if (activity_text->value().empty()) {
    return " ";
  }
  return activity_text->value();
}

}  // namespace cszb_scoreboard
