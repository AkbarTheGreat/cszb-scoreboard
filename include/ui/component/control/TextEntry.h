/*
ui/component/control/TextEntry.h: This class is responsible for generating text
which can go to one or all of the scoreboard screens.

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

#pragma once

#include <wx/wx.h>

#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"

namespace cszb_scoreboard {

class TextEntry : public ScreenTextController {
 public:
  static TextEntry *Create(PreviewPanel *preview_panel, wxWindow *parent);
  wxButton *updateButton();
  wxTextCtrl *textField();
  void textUpdated(wxKeyEvent &event);

 private:
  wxRadioBox *screen_selection;
  // If the list of screen_choices changes, be sure to change what happens in
  // textUpdated().
  wxString screen_choices[3] = {wxT("Home"), wxT("Away"), wxT("All")};
  wxStaticText *text_label;
  wxTextCtrl *text_entry;
  wxStaticText *font_size_label;
  wxTextCtrl *font_size_entry;

  TextEntry(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  void updatePreview() override;
  void createControls(wxPanel *control_panel) override;
  proto::ScreenSide updateSide() override;

  void bindEvents();
  int enteredFontSize();
  void positionWidgets(wxPanel *control_panel);
  void screenChanged(wxCommandEvent &event);
};

}  // namespace cszb_scoreboard
