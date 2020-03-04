/*
ui/component/control/things_mode/ActivityPanel.cpp: Represents all activities in
5/6 things.

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

#include "ui/component/control/things_mode/ActivityPanel.h"

#include "ScoreboardCommon.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const int ACTIVITIES_FOR_SIZING = 3;
const int INITIAL_NUMBER_OF_ACTIVITIES = 5;

ActivityPanel::ActivityPanel(wxWindow *parent,
                             ScreenTextController *owning_controller)
    : wxPanel(parent) {
  assert(INITIAL_NUMBER_OF_ACTIVITIES >= ACTIVITIES_FOR_SIZING);
  this->owning_controller = owning_controller;
  this->parent = parent;
  activity_side = new wxPanel(this);
  replacement_side = new wxPanel(this);
  bool is_first = true;
  for (int i = 0; i < ACTIVITIES_FOR_SIZING; i++) {
    activities.push_back(
        new Activity(this, activity_side, replacement_side, is_first));
    is_first = false;
  }
  bindEvents();
  positionWidgets();

  for (int i = 0; i < INITIAL_NUMBER_OF_ACTIVITIES - ACTIVITIES_FOR_SIZING;
       i++) {
    activities.push_back(
        new Activity(this, activity_side, replacement_side, is_first));
    is_first = false;
    activity_side->GetSizer()->Add(activities.back()->controlPane(), 0, wxALL,
                                   BORDER_SIZE);
    replacement_side->GetSizer()->Add(activities.back()->replacementPanel(), 0,
                                      wxALL, BORDER_SIZE);
    activities.back()->replacementPanel()->Hide();
  }
}

ActivityPanel::~ActivityPanel() {
  for (auto activity : activities) {
    delete activity;
  }
}

void ActivityPanel::bindEvents() {}

void ActivityPanel::positionWidgets() {
  wxFlexGridSizer *activity_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  activity_sizer->SetFlexibleDirection(wxBOTH);
  activity_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *replacement_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  replacement_sizer->SetFlexibleDirection(wxBOTH);
  replacement_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  for (auto activity : activities) {
    activity_sizer->Add(activity->controlPane(), 0, wxALL, BORDER_SIZE);
    replacement_sizer->Add(activity->replacementPanel(), 0, wxALL, BORDER_SIZE);
    if (!activity->isSelected()) {
      activity->replacementPanel()->Hide();
    }
  }

  activity_side->SetSizerAndFit(activity_sizer);
  replacement_side->SetSizerAndFit(replacement_sizer);

  wxFlexGridSizer *outer_sizer = new wxFlexGridSizer(0, 2, 0, 0);
  outer_sizer->SetFlexibleDirection(wxBOTH);
  outer_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  outer_sizer->Add(activity_side, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(replacement_side, 0, wxALL, BORDER_SIZE);
  SetSizerAndFit(outer_sizer);
}

void ActivityPanel::addActivity(wxPanel *parent_panel) {
  bool is_first = (activities.empty());
  activities.push_back(
      new Activity(this, activity_side, replacement_side, is_first));
  activity_side->GetSizer()->Add(activities.back()->controlPane(), 0, wxALL,
                                 BORDER_SIZE);
  replacement_side->GetSizer()->Add(activities.back()->replacementPanel(), 0,
                                    wxALL, BORDER_SIZE);
  activities.back()->select();
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
  for (auto activity : activities) {
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
      delete activity;
      activities.erase(activities.begin() + offset);
      updateNotify();
      return;
    }
    offset++;
  }
}

void ActivityPanel::selectionChanged(wxCommandEvent &event) {
  wxObject *event_object = event.GetEventObject();
  for (auto activity : activities) {
    if (activity->resolveSelection(event_object)) {
      activity->replacementPanel()->Show();
    } else {
      activity->replacementPanel()->Hide();
    }
  }
  updateNotify();
}

void ActivityPanel::textUpdated(wxKeyEvent &event) { updateNotify(); }

void ActivityPanel::updateNotify() {
  refreshSizers();
  owning_controller->updatePreview();
}

void ActivityPanel::refreshSizers() {
  replacement_side->SetSizerAndFit(replacement_side->GetSizer());
  activity_side->SetSizerAndFit(activity_side->GetSizer());
  SetSizerAndFit(GetSizer());
}

ReplacementPanel *ActivityPanel::replacementPanel() {
  if (activities.empty()) {
    return nullptr;
  }

  for (auto activity : activities) {
    if (activity->isSelected()) {
      return activity->replacementPanel();
    }
  }

  // Should be unreachable.
  return activities[0]->replacementPanel();
}

std::vector<proto::RenderableText> ActivityPanel::previewText(int font_size) {
  std::string preview_text;
  for (auto activity : activities) {
    preview_text += activity->previewText() + "\n";
  }
  std::vector<proto::RenderableText> return_vector;
  return_vector.push_back(proto::RenderableText());
  return_vector.back().set_text(preview_text);
  return_vector.back().mutable_font()->set_size(font_size);
  return return_vector;
}

}  // namespace cszb_scoreboard
