/*
ui/component/control/TextEntry.h: This class is responsible for generating text
which can go to one or all of the scoreboard screens.

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

#include <wx/clrpicker.h>
#include <wx/wx.h>

#include "ScoreboardCommon.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/control/ScreenTextController.h"
#include "ui/component/control/TeamSelector.h"

namespace cszb_scoreboard {

class TextEntry : public ScreenTextController {
 public:
  static TextEntry *Create(PreviewPanel *preview_panel, wxWindow *parent);
  wxTextCtrl *textField();
  void textUpdated(wxKeyEvent &event);

PUBLIC_TEST_ONLY
  void selectTeam(int index);

 private:
  wxColourPickerCtrl *color_picker;
  wxStaticText *font_size_label;
  wxPanel *inner_panel;
  wxTextCtrl *font_size_entry;
  TeamSelector *screen_selection;
  wxStaticText *text_label;
  wxTextCtrl *text_entry;
  wxString home_text;
  wxString away_text;
  wxString all_text;
  int home_font_size;
  int away_font_size;
  int all_font_size;
  Color home_color;
  Color away_color;
  Color all_color;

  TextEntry(PreviewPanel *preview_panel, wxWindow *parent);
  void updateScreenText(ScreenText *screen_text) override;
  void createControls(wxPanel *control_panel) override;

  void bindEvents();
  void colorChanged(wxColourPickerEvent &event);
  int enteredFontSize();
  void positionWidgets(wxPanel *control_panel);
  void screenChanged(wxCommandEvent &event);
  void doScreenChanged();
};

}  // namespace cszb_scoreboard
