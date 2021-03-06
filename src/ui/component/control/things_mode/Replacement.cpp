/*
ui/component/control/things_mode/Replacement.cpp: Represents a replacement in
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

#include "ui/component/control/things_mode/Replacement.h"

#include <wx/wx.h>

#include <vector>

#include "ui/UiUtil.h"
#include "ui/component/control/things_mode/ReplacementPanel.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
static const char *BULLET = "\u2022";

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

void Replacement::copyFrom(Replacement *other) {
  replaceable->SetValue(other->replaceable->GetValue());
  replacement->SetValue(other->replacement->GetValue());
}

void Replacement::bindEvents() {
  remove_replacement_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                                  &ReplacementPanel::deleteReplacement,
                                  dynamic_cast<ReplacementPanel *>(parent));
  replaceable->Bind(wxEVT_KEY_UP, &ReplacementPanel::textUpdated,
                    dynamic_cast<ReplacementPanel *>(parent));
  replacement->Bind(wxEVT_KEY_UP, &ReplacementPanel::textUpdated,
                    dynamic_cast<ReplacementPanel *>(parent));
}

void Replacement::positionWidgets() {
  wxSizer *sizer = UiUtil::sizer(0, 4);
  sizer->Add(replaceable, 0, wxALL, BORDER_SIZE);
  sizer->Add(replacement, 0, wxALL, BORDER_SIZE);
  sizer->Add(remove_replacement_button, 0, wxALL, BORDER_SIZE);
  sizer->Add(spacer_text, 0, wxALL, BORDER_SIZE);
  control_pane->SetSizerAndFit(sizer);
}

auto Replacement::containsDeleteButton(wxObject *delete_button) -> bool {
  return (delete_button == remove_replacement_button);
}

auto Replacement::previewText() -> std::string {
  if (replaceable->GetValue().empty() && replacement->GetValue().empty()) {
    return " ";
  }
  return std::string(BULLET) + " " + replaceable->GetValue().ToStdString() +
         " - " + replacement->GetValue().ToStdString();
}

}  // namespace cszb_scoreboard
