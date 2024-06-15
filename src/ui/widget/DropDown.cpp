/*
ui/widget/DropDown.cpp: A drop-down selector to pick one from many strings.

Copyright 2023-2024 Tracy Beck

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

#include "ui/widget/DropDown.h"

#include <wx/defs.h>    // for wxNOT_FOUND
#include <wx/string.h>  // for wxString

namespace cszb_scoreboard {

DropDown::DropDown(swx::Choice *choice,
                   const std::vector<std::string> &choices) {
  _wx = choice;
  for (const auto &choice : choices) {
    _wx->AppendString(choice);
  }
}

auto DropDown::selected() -> std::string {
  int32_t selection = _wx->GetSelection();
  if (selection == wxNOT_FOUND) {
    return "";
  }
  return _wx->GetString(selection).ToStdString();
}

void DropDown::setSelected(int32_t selection) { _wx->SetSelection(selection); }

}  // namespace cszb_scoreboard
