/*
ui/component/control/things_mode/ReplacementPanel.cpp: Represents all
replacements for one activity in 5/6 things.

Copyright 2019-2022 Tracy Beck

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

#include <string>  // for operator+
#include <vector>  // for vector

#include "ScoreboardCommon.h"                                // for DEFAULT_...
#include "config.pb.h"                                       // for Renderab...
#include "ui/component/control/things_mode/ActivityPanel.h"  // for Activity...

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;
const int INITIAL_NUMBER_OF_REPLACEMENTS = 2;

ReplacementPanel::ReplacementPanel(swx::Panel *wx,
                                   ActivityPanel *activity_panel)
    : Panel(wx) {
  should_self_delete = true;
  this->activity_panel = activity_panel;
  for (int i = 0; i < INITIAL_NUMBER_OF_REPLACEMENTS; i++) {
    replacements.push_back(std::make_unique<Replacement>(this));
  }
  bindEvents();
  positionWidgets();
}

void ReplacementPanel::bindEvents() {}

void ReplacementPanel::positionWidgets() {
  int row = 0;
  for (const auto &replacement : replacements) {
    addWidget(*replacement->controlPane(), row++, 0);
  }
  runSizer();
}

void ReplacementPanel::addReplacement() {
  replacements.push_back(std::make_unique<Replacement>(this));
  addWidget(*(replacements.back()->controlPane()), replacements.size() - 1, 0);
  updateNotify();
}

void ReplacementPanel::deleteReplacement(Replacement *deleted) {
  int offset = 0;
  for (const auto &replacement : replacements) {
    if (replacement.get() == deleted) {
      removeRowFromSizer(offset);
      replacements.erase(replacements.begin() + offset);
      updateNotify();
      return;
    }
    offset++;
  }
}

auto ReplacementPanel::previewText(int font_size)
    -> std::vector<proto::RenderableText> {
  std::string preview_text = activity_panel->selectedActivityText() + "\n \n";

  for (const auto &replacement : replacements) {
    preview_text += replacement->previewText() + "\n";
  }
  std::vector<proto::RenderableText> return_vector;
  return_vector.emplace_back(proto::RenderableText());
  return_vector.back().set_text(preview_text);
  return_vector.back().mutable_font()->set_size(font_size);
  return return_vector;
}

void ReplacementPanel::textUpdated() { updateNotify(); }

void ReplacementPanel::updateNotify() {
  runSizer();
  activity_panel->updateNotify();
}

}  // namespace cszb_scoreboard
