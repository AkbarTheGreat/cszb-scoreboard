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

Replacement::Replacement(ReplacementPanel *parent) {
  this->parent = parent;
  control_pane = parent->panel(true);
  replaceable = control_pane->text("", true);
  replacement = control_pane->text("", true);
  remove_replacement_button = control_pane->button("X", true);
  // We add a blank space to the end of the line so that when the scrollbar
  // appears it doesn't occlude the delete button.
  spacer_text = control_pane->label("   ");
  bindEvents();
  positionWidgets();
}

void Replacement::copyFrom(Replacement *other) {
  replaceable->setValue(other->replaceable->value());
  replacement->setValue(other->replacement->value());
}

void Replacement::bindEvents() {
  auto *rp = dynamic_cast<ReplacementPanel *>(parent);
  remove_replacement_button->bind(wxEVT_COMMAND_BUTTON_CLICKED,
                                  [this, rp](wxCommandEvent &event) -> void {
                                    rp->deleteReplacement(this);
                                  });
  replaceable->bind(wxEVT_KEY_UP,
                    [rp](wxKeyEvent &event) -> void { rp->textUpdated(); });
  replacement->bind(wxEVT_KEY_UP,
                    [rp](wxKeyEvent &event) -> void { rp->textUpdated(); });
}

void Replacement::positionWidgets() {
  control_pane->addWidget(*replaceable, 0, 0);
  control_pane->addWidget(*replacement, 0, 1);
  control_pane->addWidget(*remove_replacement_button, 0, 2);
  control_pane->addWidget(*spacer_text, 0, 3);
  control_pane->runSizer();
}

auto Replacement::previewText() -> std::string {
  if (replaceable->value().empty() && replacement->value().empty()) {
    return " ";
  }
  return std::string(BULLET) + " " + replaceable->value() + " - " +
         replacement->value();
}

}  // namespace cszb_scoreboard
