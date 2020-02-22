/*
ui/component/control/things_mode/Activity.h: Represents an activity in 5/6
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

#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/things_mode/ReplacementPanel.h"

namespace cszb_scoreboard {
class Activity {
 public:
  Activity(wxWindow *parent, wxPanel *activity_frame,
           wxPanel *replacement_frame, bool is_first);
  wxPanel *controlPane() { return control_pane; }
  bool isSelected();
  ReplacementPanel *replacementPanel() { return replacement_panel; }
  bool resolveSelection(wxObject *selected_object);
  void select();
  void unselect();

 private:
  wxPanel *control_pane;
  wxRadioButton *activity_selector;
  wxTextCtrl *activity_text;
  wxWindow *parent;
  wxButton *remove_activity_button;
  ReplacementPanel *replacement_panel;

  void bindEvents();
  void positionWidgets();
  void selectionChanged(wxCommandEvent &event);
};

}  // namespace cszb_scoreboard
