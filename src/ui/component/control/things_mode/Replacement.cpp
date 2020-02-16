/*
ui/component/control/things_mode/Replacement.h: Represents a replacement in 5/6
things.

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

#include <wx/wx.h>

#include <vector>

#include "ui/component/control/things_mode/Replacement.h"

namespace cszb_scoreboard {

Replacement::Replacement(wxWindow *parent,
                         ScreenTextController *owning_controller) {
  this->owning_controller = owning_controller;
  replaceable = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, -1), wxTE_MULTILINE);
  replacement = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, -1), wxTE_MULTILINE);
  remove_replacement_button = new wxButton(
      parent, wxID_ANY, "X", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
  // We add a blank space to the end of the line so that when the scrollbar
  // appears it doesn't occlude the delete button.
  spacer_text = new wxStaticText(parent, wxID_ANY, "   ");
  bindEvents();
}

std::vector<wxWindow *> Replacement::line() {
  std::vector<wxWindow *> list_of_widgets;
  list_of_widgets.push_back(replaceable);
  list_of_widgets.push_back(replacement);
  list_of_widgets.push_back(remove_replacement_button);
  list_of_widgets.push_back(spacer_text);
  return list_of_widgets;
}

int Replacement::lineWidth() { return 4; }

void Replacement::bindEvents() {}

void Replacement::deleteReplacement(wxCommandEvent &event) {}

}  // namespace cszb_scoreboard
