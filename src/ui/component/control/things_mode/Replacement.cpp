/*
ui/component/control/things_mode/Replacement.cpp: Represents a replacement in
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

#include "ui/component/control/things_mode/Replacement.h"

#include <wx/wx.h>

#include <vector>

#include "ui/component/control/things_mode/ReplacementPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

Replacement::Replacement(wxWindow *parent) {
  this->parent = parent;
  control_pane = new wxPanel(parent);
  replaceable = new wxTextCtrl(control_pane, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, -1), wxTE_MULTILINE);
  replacement = new wxTextCtrl(control_pane, wxID_ANY, "", wxDefaultPosition,
                               wxSize(-1, -1), wxTE_MULTILINE);
  remove_replacement_button =
      new wxButton(control_pane, wxID_ANY, "X", wxDefaultPosition,
                   wxDefaultSize, wxBU_EXACTFIT);
  // We add a blank space to the end of the line so that when the scrollbar
  // appears it doesn't occlude the delete button.
  spacer_text = new wxStaticText(control_pane, wxID_ANY, "   ");
  bindEvents();
  positionWidgets();
}

Replacement::~Replacement() { control_pane->Destroy(); }

void Replacement::bindEvents() {
  remove_replacement_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                                  &ReplacementPanel::deleteReplacement,
                                  (ReplacementPanel *)parent);
}

void Replacement::positionWidgets() {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 4, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  sizer->Add(replaceable, 0, wxALL, BORDER_SIZE);
  sizer->Add(replacement, 0, wxALL, BORDER_SIZE);
  sizer->Add(remove_replacement_button, 0, wxALL, BORDER_SIZE);
  sizer->Add(spacer_text, 0, wxALL, BORDER_SIZE);
  control_pane->SetSizerAndFit(sizer);
}

bool Replacement::containsDeleteButton(wxObject *delete_button) {
  if (delete_button == remove_replacement_button) {
    return true;
  } else {
    return false;
  }
}

void Replacement::deleteReplacement(wxCommandEvent &event) {}

}  // namespace cszb_scoreboard
