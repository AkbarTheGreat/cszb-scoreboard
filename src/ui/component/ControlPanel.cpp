/*
ui/component/ControlPanel.cpp: Holds the controls for changing the contents on a
screen.

Copyright 2019 Tracy Beck

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

#include "ui/component/ControlPanel.h"

namespace cszb_scoreboard {

ControlPanel::ControlPanel(wxWindow* parent, PreviewPanel* preview_panel)
    : wxNotebook(parent, wxID_ANY) {
  text_entry = TextEntry::Create(preview_panel, this);
  AddPage(text_entry, "Text");

  score_control = ScoreControl::Create(preview_panel, this);
  AddPage(score_control, "Score");
}

}  // namespace cszb_scoreboard
