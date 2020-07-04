/*
ui/component/control/things_mode/Replacement.h: Represents a replacement in 5/6
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

namespace cszb_scoreboard {
class Replacement {
 public:
  Replacement(wxWindow *parent);
  ~Replacement();
  void copyFrom(Replacement *other);
  bool containsDeleteButton(wxObject *delete_button);
  wxPanel *controlPane() { return control_pane; }
  wxButton *deleteButton() { return remove_replacement_button; }
  std::string previewText();

 private:
  wxPanel *control_pane;
  wxWindow *parent;
  wxTextCtrl *replaceable;
  wxTextCtrl *replacement;
  wxButton *remove_replacement_button;
  wxStaticText *spacer_text;

  void bindEvents();
  void positionWidgets();
};
}  // namespace cszb_scoreboard