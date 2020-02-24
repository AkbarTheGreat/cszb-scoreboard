/*
ui/component/control/things_mode/ReplacementPanel.cpp: Represents all
replacements for one activity in 5/6 things.

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

#include "ui/component/control/things_mode/ReplacementPanel.h"

#include <wx/wx.h>

#include <vector>

#include "ui/component/control/things_mode/ActivityPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ReplacementPanel::ReplacementPanel(wxWindow *parent, wxWindow *activity_panel)
    : wxPanel(parent) {
  // TODO: Research custom events and throw an event upward to propigate these
  // changes rather than a much more brittle forced-cast relationship.
  this->activity_panel = activity_panel;
  replacements.push_back(new Replacement(this));
  bindEvents();
  positionWidgets();
}

ReplacementPanel::~ReplacementPanel() {
  for (auto replacement : replacements) {
    delete replacement;
  }
}

void ReplacementPanel::bindEvents() {}

void ReplacementPanel::positionWidgets() {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto replacement : replacements) {
    sizer->Add(replacement->controlPane(), 0, wxALL, BORDER_SIZE);
  }
  SetSizerAndFit(sizer);
}

void ReplacementPanel::deleteReplacement(wxCommandEvent &event) {
  wxObject *event_object = event.GetEventObject();
  int offset = 0;
  for (auto replacement : replacements) {
    if (replacement->containsDeleteButton(event_object)) {
      delete replacement;
      replacements.erase(replacements.begin() + offset);
      ((ActivityPanel *)activity_panel)->updateNotify();
      return;
    }
    offset++;
  }
}

}  // namespace cszb_scoreboard
