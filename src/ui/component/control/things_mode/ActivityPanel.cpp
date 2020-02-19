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

ActivityPanel::ActivityPanel(wxWindow *parent,
                             ScreenTextController *owning_controller)
    : wxPanel(parent) {
  aui_manager.SetManagedWindow(this);
  this->owning_controller = owning_controller;
  this->parent = parent;
  activities.push_back(Activity(this, true));
  bindEvents();
  positionWidgets();
}

void ActivityPanel::addActivity(wxPanel *parent_panel) {
  bool is_first = (activities.empty());
  activities.push_back(Activity(this, is_first));
  addToAui(activities.back().controlPane(), true);

  activities.back().select();
  selectionChanged(wxCommandEvent());
  SetSize(517,192);
}

void ActivityPanel::selectionChanged(wxCommandEvent &event) {
  for (auto activity : activities) {
    if (activity.isSelected()) {
      addToAui(activity.replacementPanel());
    } else {
      aui_manager.DetachPane(activity.replacementPanel());
    }
  }
  aui_manager.Update();
  owning_controller->updatePreview();
}

void ActivityPanel::bindEvents() {}

ReplacementPanel *ActivityPanel::replacementPanel() {
  if (activities.empty()) {
    return nullptr;
  }

  for (auto activity : activities) {
    if (activity.isSelected()) {
      return activity.replacementPanel();
    }
  }

  // Should be unreachable.
  return activities[0].replacementPanel();
}

void ActivityPanel::addToAui(wxWindow *widget, bool on_left, int row) {
  wxAuiPaneInfo pane_info;
  // pane_info.CenterPane();

  if (on_left) {
    pane_info.Left();
  } else {
    pane_info.Right();
  }

  pane_info.MinSize(widget->GetSize());
  aui_manager.AddPane(widget, pane_info);
}

void ActivityPanel::positionWidgets() {
  addToAui(replacementPanel());

  for (auto activity : activities) {
    addToAui(activity.controlPane(), true);
  }

  wxSize my_size(
      replacementPanel()->GetSize().GetWidth() +
          activities.back().controlPane()->GetSize().GetWidth(),
      std::max(replacementPanel()->GetSize().GetHeight(),
               activities.back().controlPane()->GetSize().GetHeight()));

  wxLogDebug("My size: %dx%d", my_size.GetWidth(), my_size.GetHeight());
  wxLogDebug("Replacement panel size: %dx%d",
             replacementPanel()->GetSize().GetWidth(),
             replacementPanel()->GetSize().GetHeight());
  wxLogDebug("Controls panel size: %dx%d",
             activities.back().controlPane()->GetSize().GetWidth(),
             activities.back().controlPane()->GetSize().GetHeight());
  wxLogDebug("Activity panel size: %dx%d", GetSize().GetWidth(),
             GetSize().GetHeight());

  SetSizeHints(my_size);
  aui_manager.Update();
  wxLogDebug("Replacement panel size: %dx%d",
             replacementPanel()->GetSize().GetWidth(),
             replacementPanel()->GetSize().GetHeight());
  wxLogDebug("Controls panel size: %dx%d",
             activities.back().controlPane()->GetSize().GetWidth(),
             activities.back().controlPane()->GetSize().GetHeight());
  wxLogDebug("Activity panel size: %dx%d", GetSize().GetWidth(),
             GetSize().GetHeight());
  SetSize(my_size);
}

}  // namespace cszb_scoreboard
