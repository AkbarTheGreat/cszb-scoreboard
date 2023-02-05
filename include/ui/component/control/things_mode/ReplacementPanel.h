/*
ui/component/control/things_mode/ReplacementPanel.h: Represents all replacements
for one activity in 5/6 things.

Copyright 2019-2023 Tracy Beck

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

#include <memory>  // for unique_ptr
#include <vector>  // for vector

#include "ui/component/control/things_mode/Replacement.h"  // for Replacement
#include "ui/widget/Panel.h"                               // for Panel

namespace cszb_scoreboard {

// Pre-defining ActivityPanel for a parent pointer.
class ActivityPanel;

namespace proto {
class RenderableText;
}  // namespace proto
namespace swx {
class Panel;
}  // namespace swx

class ReplacementPanel : public Panel {
 public:
  ReplacementPanel(swx::Panel *wx, ActivityPanel *activity_panel);
  void addReplacement();
  void deleteReplacement(Replacement *deleted);
  auto previewText(int font_size) -> std::vector<proto::RenderableText>;
  void textUpdated();

 private:
  ActivityPanel *activity_panel;
  std::vector<std::unique_ptr<Replacement>> replacements;

  void bindEvents();
  void positionWidgets();
  void updateNotify();
};
}  // namespace cszb_scoreboard
