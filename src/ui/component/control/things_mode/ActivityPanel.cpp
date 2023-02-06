/*
ui/component/control/things_mode/ActivityPanel.cpp: Represents all activities in
5/6 things.

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

#include "ui/component/control/things_mode/ActivityPanel.h"

#include <cassert>  // for assert

#include "ScoreboardCommon.h"                                   // for DEFAU...
#include "config/swx/event.h"                                   // for wxEVT...
#include "ui/component/control/ScreenTextController.h"          // for Scree...
#include "ui/component/control/things_mode/ReplacementPanel.h"  // for Repla...
#include "ui/graphics/TeamColors.h"                             // for TeamC...
#include "ui/widget/Widget.h"                                   // for NO_BO...

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const int ACTIVITIES_FOR_SIZING = 3;
const int INITIAL_NUMBER_OF_ACTIVITIES = 5;
// Just a buffer for adding new replacements to ensure they can't collide with
// existing ones.  In theory this could be 1, but it self-resolves pretty much
// immediately whent they're repositioned, so it doesn't matter much.
const int REPLACEMENT_BUFFER_SIZE = 5;
static const char *BULLET = "\u2022-2023";

ActivityPanel::ActivityPanel(swx::Panel *wx,
                             ScreenTextController *owning_controller,
                             const proto::ScreenSide &side,
                             Singleton *singleton)
    : Panel(wx) {
  assert(INITIAL_NUMBER_OF_ACTIVITIES >= ACTIVITIES_FOR_SIZING);
  this->owning_controller = owning_controller;
  this->side = side;
  this->singleton = singleton;
  activity_label = labelledArea("Activities");
  replacement_label = labelledArea("Replacements");
  activity_half = activity_label->holds()->panel();
  replacement_half = replacement_label->holds()->panel();
  color_picker = colorPicker(singleton->teamColors()->getColor(side));

  // Add only as many activites as we want the initial pane size to be sized
  // for.
  bool is_first = true;
  for (int i = 0; i < ACTIVITIES_FOR_SIZING; i++) {
    activities.push_back(std::make_unique<Activity>(
        this, activity_half.get(), replacement_half.get(), i, is_first));
    is_first = false;
  }
  bindEvents();
  positionWidgets();

  // Add the activities which we don't size the scrollbar for.
  for (int i = ACTIVITIES_FOR_SIZING; i < INITIAL_NUMBER_OF_ACTIVITIES; i++) {
    activities.push_back(std::make_unique<Activity>(
        this, activity_half.get(), replacement_half.get(), i, is_first));
    is_first = false;
    activity_half->addWidget(*activities.back()->controlPane(), i, 0);
    replacement_half->addWidget(*activities.back()->replacementPanel(),
                                i + REPLACEMENT_BUFFER_SIZE, 0);
    activities.back()->replacementPanel()->hide();
  }
  resetActivityMoveButtons();
}

void ActivityPanel::bindEvents() {
  color_picker->bind(
      wxEVT_COLOURPICKER_CHANGED,
      [this](wxColourPickerEvent &event) -> void { this->colorChanged(); });
}

void ActivityPanel::positionWidgets() {
  int row = 0;
  int first_activity = 0;
  for (const auto &activity : activities) {
    if (activity->isSelected()) {
      first_activity = row;
    }
    activity_half->addWidget(*activity->controlPane(), row, 0);
    replacement_half->addWidget(*activity->replacementPanel(),
                                row + REPLACEMENT_BUFFER_SIZE, 0);
    row++;
  }
  showReplacement(first_activity);

  activity_label->holds()->addWidget(*activity_half, 0, 0, NO_BORDER);
  replacement_label->holds()->addWidget(*replacement_half, 0, 1, NO_BORDER);

  addWidget(*activity_label, 0, 0);
  addWidget(*replacement_label, 0, 1);
  addWidget(*color_picker, 1, 0);
  runSizer();
}

void ActivityPanel::addActivity() {
  bool is_first = (activities.empty());
  activities.push_back(std::make_unique<Activity>(this, activity_half.get(),
                                                  replacement_half.get(),
                                                  activities.size(), is_first));
  activity_half->addWidget(*activities.back()->controlPane(),
                           activities.size() - 1, 0);
  replacement_half->addWidget(*activities.back()->replacementPanel(),
                              activities.size() + REPLACEMENT_BUFFER_SIZE, 0);
  activities.back()->select();

  resetActivityMoveButtons();
  updateNotify();
}

void ActivityPanel::addReplacement() {
  if (replacementPanel() != nullptr) {
    replacementPanel()->addReplacement();
    updateNotify();
  }
}

void ActivityPanel::deleteActivity(Activity *deleted) {
  int offset = 0;
  for (const auto &activity : activities) {
    if (activity.get() == deleted) {
      if (activity->isSelected()) {
        if (activities.size() > 1) {
          if (offset == 0) {
            activities[1]->select();
          } else {
            activities[0]->select();
          }
        }
        hideAllReplacements();
      }
      activity_half->removeRowFromSizer(offset);
      replacement_half->removeRowFromSizer(offset + 1);
      activities.erase(activities.begin() + offset);
      showSelectedReplacement();
      resetActivityMoveButtons();
      updateNotify();
      return;
    }
    offset++;
  }
}

void ActivityPanel::selectionChanged(Activity *selected) {
  for (int i = 0; i < activities.size(); i++) {
    if (activities[i].get() == selected) {
      activities[i]->select();
      showReplacement(i);
    } else {
      activities[i]->unselect();
    }
  }
  updateNotify();
}

void ActivityPanel::textUpdated() { updateNotify(); }

void ActivityPanel::updateNotify() {
  refreshSizers();
  owning_controller->updatePreview();
}

void ActivityPanel::colorChanged() {
  singleton->teamColors()->setColor(side, color_picker->color());

  owning_controller->updatePreview();
}

void ActivityPanel::refreshSizers() {
  replacement_half->runSizer();
  activity_half->runSizer();
  runSizer();
}

void ActivityPanel::swapActivities(int a, int b) {
  hideAllReplacements();
  Activity temp(this, activity_half.get(), replacement_half.get(), 0, false);
  temp.copyFrom(activities[a].get());
  activities[a]->copyFrom(activities[b].get());
  activities[b]->copyFrom(&temp);

  // Move these out of each other's way for when the replacement hide/show logic
  // runs.
  int offset = activities.size() + REPLACEMENT_BUFFER_SIZE;
  replacement_half->moveWidget(activities[a]->replacementPanel(), offset + a,
                               0);
  replacement_half->moveWidget(activities[b]->replacementPanel(), offset + b,
                               0);

  showSelectedReplacement();

  updateNotify();
}

auto ActivityPanel::getColor() -> Color {
  color_picker->setColor(singleton->teamColors()->getColor(side));
  return singleton->teamColors()->getColor(side);
}

auto ActivityPanel::replacementPanel() -> ReplacementPanel * {
  if (activities.empty()) {
    return nullptr;
  }

  for (const auto &activity : activities) {
    if (activity->isSelected()) {
      return activity->replacementPanel();
    }
  }

  // Should be unreachable.
  return activities[0]->replacementPanel();
}

auto ActivityPanel::selectedActivityText() -> std::string {
  for (const auto &activity : activities) {
    if (activity->isSelected()) {
      return activity->previewText();
    }
  }
  return " ";
}

auto ActivityPanel::previewText(int font_size)
    -> std::vector<proto::RenderableText> {
  std::string preview_text;
  for (const auto &activity : activities) {
    preview_text += std::string(BULLET) + " " + activity->previewText() + "\n";
  }
  std::vector<proto::RenderableText> return_vector;
  return_vector.emplace_back();
  return_vector.back().set_text(preview_text);
  return_vector.back().mutable_font()->set_size(font_size);
  return return_vector;
}

void ActivityPanel::resetActivityMoveButtons() {
  for (int i = 0; i < activities.size(); i++) {
    activities[i]->setIndex(i, activities.size() - 1);
  }
}

void ActivityPanel::hideAllReplacements() {
  for (int i = 0; i < activities.size(); i++) {
    activities[i]->replacementPanel()->hide();
    replacement_half->moveWidget(activities[i]->replacementPanel(), i + 1, 0);
  }
}

void ActivityPanel::showSelectedReplacement() {
  for (int i = 0; i < activities.size(); i++) {
    if (activities[i]->isSelected()) {
      showReplacement(i);
      return;
    }
  }
}

void ActivityPanel::showReplacement(int index) {
  hideAllReplacements();
  replacement_half->moveWidget(activities[index]->replacementPanel(), 0, 0);
  activities[index]->replacementPanel()->show();
}

}  // namespace cszb_scoreboard
