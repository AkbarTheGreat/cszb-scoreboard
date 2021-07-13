/*
ui/component/control/things_mode/Replacement.h: Represents a replacement in 5/6
things.

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

#pragma once

#include <memory>              // for unique_ptr
#include <string>              // for string

#include "ui/widget/Button.h"  // for Button
#include "ui/widget/Label.h"   // for Label
#include "ui/widget/Panel.h"   // for Panel
#include "ui/widget/Text.h"    // for Text

namespace cszb_scoreboard {

// Pre-defining ReplacementPanel for a parent pointer.
class ReplacementPanel;

class Replacement {
 public:
  explicit Replacement(ReplacementPanel *parent);
  void copyFrom(Replacement *other);
  auto controlPane() -> Panel * { return control_pane.get(); }
  auto deleteButton() -> Button * { return remove_replacement_button.get(); }
  auto previewText() -> std::string;

 private:
  std::unique_ptr<Panel> control_pane;
  std::unique_ptr<Text> replaceable, replacement;
  std::unique_ptr<Button> remove_replacement_button;
  std::unique_ptr<Label> spacer_text;
  ReplacementPanel *parent;

  void bindEvents();
  void positionWidgets();
};
}  // namespace cszb_scoreboard
