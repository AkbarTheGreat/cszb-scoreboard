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

#include <array>

namespace cszb_scoreboard {

static const std::array<wxString, 3> SCREEN_CHOICES{
    {{"Home"}, {"Away"}, {"All"}}};

TeamSelector::TeamSelector(wxWindow* parent)
    : wxRadioBox(parent, wxID_ANY, wxT("Team"), wxDefaultPosition,
                 wxDefaultSize, SCREEN_CHOICES.size(), SCREEN_CHOICES.data(), 1,
                 wxRA_SPECIFY_COLS) {
  SetSelection(0);
}

TeamSelector::TeamSelector(wxWindow* parent, const proto::ScreenSide& side)
    : TeamSelector(parent) {
  if (side.home() && side.away()) {
    SetSelection(2);
  } else if (side.away()) {
    SetSelection(1);
  } else if (side.home()) {
    SetSelection(0);
  }
}

auto TeamSelector::allSelected() -> bool { return (GetSelection() == 2); }

auto TeamSelector::awaySelected() -> bool {
  return (GetSelection() == 1 || GetSelection() == 2);
}

auto TeamSelector::homeSelected() -> bool {
  return (GetSelection() == 0 || GetSelection() == 2);
}

}  // namespace cszb_scoreboard
