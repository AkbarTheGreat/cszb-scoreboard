/*
ui/component/control/things_mode/Activity.h: Represents an activity in 5/6
things.

Copyright 2019-2025 Tracy Beck

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

#include <memory>  // for uniqu...
#include <string>  // for string

#include "ui/component/control/things_mode/ReplacementPanel.h"  // for Repla...
#include "ui/widget/Button.h"                                   // for Button
#include "ui/widget/Panel.h"                                    // for Panel
#include "ui/widget/RadioButton.h"                              // for Radio...
#include "ui/widget/Text.h"                                     // for Text

namespace cszb_scoreboard {

// Pre-defining ActivityPanel for a parent pointer.
class ActivityPanel;

class Activity {
 public:
  Activity(ActivityPanel *parent, Panel *activity_frame,
           Panel *replacement_frame, int index, bool is_first);
  void copyFrom(Activity *other);
  auto controlPane() -> Panel * { return control_pane.get(); }
  auto previewText() -> std::string;
  auto replacementPanel() -> ReplacementPanel * {
    return replacement_panel.get();
  }
  void setIndex(int index, int max_index);
  auto isSelected() -> bool;
  void select();
  void unselect();

 private:
  int index;
  std::unique_ptr<Panel> control_pane;
  std::unique_ptr<RadioButton> activity_selector;
  std::unique_ptr<Text> activity_text;
  std::unique_ptr<Button> down_button, up_button, remove_activity_button;
  ActivityPanel *parent;
  std::unique_ptr<ReplacementPanel> replacement_panel;

  void bindEvents();
  void positionWidgets();
  void moveButton(bool is_up);
};

}  // namespace cszb_scoreboard
