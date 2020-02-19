/*
ui/component/control/things_mode/ReplacementPanel.h: Represents all replacements
for one activity in 5/6 things.

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

#pragma once

#include "ui/component/control/things_mode/ReplacementPanel.h"

#include <wx/wx.h>

#include <vector>

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ReplacementPanel::ReplacementPanel(wxWindow *parent) : wxPanel(parent) {
  replacements.push_back(Replacement(this));
  bindEvents();
  positionWidgets();
}

void ReplacementPanel::bindEvents() {}

void ReplacementPanel::positionWidgets() {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto replacement : replacements) {
    sizer->Add(replacement.controlPane(), 0, wxALL, BORDER_SIZE);
  }
  SetSizerAndFit(sizer);
}

}  // namespace cszb_scoreboard
