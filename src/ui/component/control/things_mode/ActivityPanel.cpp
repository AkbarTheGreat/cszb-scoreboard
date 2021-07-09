/*
ui/component/control/things_mode/ActivityPanel.cpp: Represents all activities in
5/6 things.

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

#include "ui/component/control/things_mode/ActivityPanel.h"

#include "ScoreboardCommon.h"
#include "ui/UiUtil.h"
#include "ui/graphics/TeamColors.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const int ACTIVITIES_FOR_SIZING = 3;
const int INITIAL_NUMBER_OF_ACTIVITIES = 5;
static const char *BULLET = "\u2022";

ActivityPanel::ActivityPanel(swx::Panel *wx,
                             ScreenTextController *owning_controller,
                             const proto::ScreenSide &side)
    : Panel(wx) {
  assert(INITIAL_NUMBER_OF_ACTIVITIES >= ACTIVITIES_FOR_SIZING);
  this->owning_controller = owning_controller;
  this->side = side;
  activity_half = panel();
  replacement_half = panel();
  color_picker = colorPicker(TeamColors::getInstance()->getColor(side));

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
    UiUtil::addToGridBag(activity_half->sizer(),
                         activities.back()->controlPane(), i, 0);
    UiUtil::addToGridBag(replacement_half->sizer(),
                         activities.back()->replacementPanel(), i + 1, 0);
    activities.back()->replacementPanel()->Hide();
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
    UiUtil::addToGridBag(activity_half->sizer(), activity->controlPane(), row,
                         0);
    UiUtil::addToGridBag(replacement_half->sizer(),
                         activity->replacementPanel(), ++row, 0);
  }
  showReplacement(first_activity);

  activity_half->runSizer();
  replacement_half->runSizer();

  //  wxSizer *outer_sizer = UiUtil::sizer(0, 2);
  addWidget(*activity_half, 0, 0);
  addWidget(*replacement_half, 0, 1);
  addWidget(*color_picker, 1, 0);
  runSizer();
}

void ActivityPanel::addActivity(wxPanel *parent_panel) {
  bool is_first = (activities.empty());
  activities.push_back(std::make_unique<Activity>(this, activity_half.get(),
                                                  replacement_half.get(),
                                                  activities.size(), is_first));
  UiUtil::addToGridBag(activity_half->sizer(), activities.back()->controlPane(),
                       activities.size() - 1, 0);
  UiUtil::addToGridBag(replacement_half->sizer(),
                       activities.back()->replacementPanel(),
                       activities.size() + 1, 0);
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

void ActivityPanel::deleteActivity(wxCommandEvent &event) {
  wxObject *event_object = event.GetEventObject();
  int offset = 0;
  for (const auto &activity : activities) {
    if (activity->containsDeleteButton(event_object)) {
      if (activity->isSelected()) {
        if (activities.size() > 1) {
          if (offset == 0) {
            activities[1]->select();
          } else {
            activities[0]->select();
          }
        }
      }
      activity_half->removeRowFromSizer(offset);
      replacement_half->removeRowFromSizer(offset + 1);
      activities.erase(activities.begin() + offset);
      resetActivityMoveButtons();
      updateNotify();
      return;
    }
    offset++;
  }
}

void ActivityPanel::selectionChanged(wxCommandEvent &event) {
  wxObject *event_object = event.GetEventObject();
  for (int i = 0; i < activities.size(); i++) {
    if (activities[i]->resolveSelection(event_object)) {
      showReplacement(i);
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
  TeamColors::getInstance()->setColor(side, Color(color_picker->color()));

  owning_controller->updatePreview();
}

void ActivityPanel::refreshSizers() {
  replacement_half->runSizer();
  activity_half->runSizer();
  runSizer();
}

void ActivityPanel::swapActivities(int a, int b) {
  Activity temp(this, activity_half.get(), replacement_half.get(), 0, false);
  temp.copyFrom(activities[a].get());
  activities[a]->copyFrom(activities[b].get());
  activities[b]->copyFrom(&temp);

  if (activities[a]->isSelected()) {
    showReplacement(a);
  } else if (activities[b]->isSelected()) {
    showReplacement(b);
  }

  updateNotify();
}

auto ActivityPanel::getColor() -> Color {
  color_picker->setColor(TeamColors::getInstance()->getColor(side));
  return TeamColors::getInstance()->getColor(side);
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
  return_vector.emplace_back(proto::RenderableText());
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
  // Move them all way out first, then move them back to position, to avoid
  // things getting confused.
  for (int i = 0; i < activities.size(); i++) {
    replacement_half->moveWidget(activities[i]->replacementPanel(), i + 32, 0);
  }
  for (int i = 0; i < activities.size(); i++) {
    activities[i]->replacementPanel()->Hide();
    replacement_half->moveWidget(activities[i]->replacementPanel(), i + 1, 0);
  }
}

void ActivityPanel::showReplacement(int index) {
  hideAllReplacements();
  replacement_half->moveWidget(activities[index]->replacementPanel(), 0, 0);
  activities[index]->replacementPanel()->Show();
}

}  // namespace cszb_scoreboard
