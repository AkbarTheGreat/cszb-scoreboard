/*
ui/component/control/TeamSelector.cpp: A group of radio buttons which selects
which team's or teams' screen(s) to send data to.

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

#include "ui/component/control/TeamSelector.h"

namespace cszb_scoreboard {

const int NUMBER_OF_SCREEN_CHOICES = 3;
const wxString SCREEN_CHOICES[NUMBER_OF_SCREEN_CHOICES] = {
    wxT("Home"), wxT("Away"), wxT("All")};

TeamSelector::TeamSelector(wxWindow *parent)
    : wxRadioBox(parent, wxID_ANY, wxT("Team"), wxDefaultPosition,
                 wxDefaultSize, NUMBER_OF_SCREEN_CHOICES, SCREEN_CHOICES, 1,
                 wxRA_SPECIFY_COLS) {
  SetSelection(0);
}

bool TeamSelector::allSelected() { return (GetSelection() == 2); }

bool TeamSelector::awaySelected() {
  return (GetSelection() == 1 || GetSelection() == 2);
}

bool TeamSelector::homeSelected() {
  return (GetSelection() == 0 || GetSelection() == 2);
}

}  // namespace cszb_scoreboard