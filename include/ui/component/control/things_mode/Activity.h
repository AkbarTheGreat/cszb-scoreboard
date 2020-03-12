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

#include "config.pb.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/things_mode/ReplacementPanel.h"

namespace cszb_scoreboard {
class Activity {
 public:
  Activity(wxWindow *parent, wxPanel *activity_frame,
           wxPanel *replacement_frame, int index, bool is_first);
  ~Activity();
  void copyFrom(Activity *other);
  wxPanel *controlPane() { return control_pane; }
  bool containsDeleteButton(wxObject *delete_button);
  std::string previewText();
  ReplacementPanel *replacementPanel() { return replacement_panel; }
  bool resolveSelection(wxObject *selected_object);
  void setIndex(int index, int max_index);
  bool isSelected();
  void select();
  void unselect();

 private:
  int index;
  wxPanel *control_pane;
  wxRadioButton *activity_selector;
  wxTextCtrl *activity_text;
  wxButton *down_button;
  wxWindow *parent;
  wxButton *remove_activity_button;
  ReplacementPanel *replacement_panel;
  wxButton *up_button;

  void bindEvents();
  void positionWidgets();
  void moveButton(wxCommandEvent &event);
  void selectionChanged(wxCommandEvent &event);
};

}  // namespace cszb_scoreboard
