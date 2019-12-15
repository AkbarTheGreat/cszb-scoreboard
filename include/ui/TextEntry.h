/*
ui/TextEntry.h: This class is responsible for generating text which can go to
one or all of the scoreboard screens.

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

#include "ui/MainView.h"
#include "ui/ScreenSide.h"

namespace cszb_scoreboard {

// Predeclared here in order to accomodate the necessary circular dependency
// inherent in having a member with a constructor which takes a parent pointer.
// Use caution when working with these variables to ensure that overloads are
// handled correctly.
class MainView;

class TextEntry : public wxPanel {
 public:
  TextEntry(MainView *parent);
  wxButton *updateButton();
  wxTextCtrl *textField();

 private:
  void textUpdated(wxKeyEvent &event);
  void updateClicked(wxCommandEvent &event);
  void screenChanged(wxCommandEvent &event);
  MainView *parent;
  wxButton *update_screens;
  wxRadioBox *screen_selection;
  wxString screen_choices[3] = {wxT("Left"), wxT("Right"), wxT("All")};
  wxTextCtrl *text_entry;
};

}  // namespace cszb_scoreboard
