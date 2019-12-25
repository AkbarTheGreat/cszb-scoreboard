/*
ui/MainView.h: This class represents the main control window for the
application.

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

#include <vector>

#include "ui/ScreenPreview.h"
#include "ui/TextEntry.h"

namespace cszb_scoreboard {

// Predeclared here in order to accomodate the necessary circular dependency
// inherent in having a member with a constructor which takes a parent pointer.
// Use caution when working with these variables to ensure that overloads are
// handled correctly.
class TextEntry;

class MainView : public wxFrame {
 public:
  MainView(const wxString& title, const wxPoint& pos, const wxSize& size);
  inline int numPreviews() { return screens.size(); }
  inline ScreenPreview* preview(int index) {
    assert(index >= 0 && index < screens.size());
    return screens[index];
  }
  TextEntry* textEntry() { return this->text_entry; }
  void setTextForPreview(wxString text, int font_size, proto::ScreenSide side);
  void updatePresenters(proto::ScreenSide side);

 private:
  std::vector<ScreenPreview*> screens;
  TextEntry* text_entry;
  void bindEvents();
  void blackout(wxCommandEvent& event);
  void createMenu();
  void createPreviews();
  void createStatusBar();
  void onExit(wxCommandEvent& event);
  void onAbout(wxCommandEvent& event);
  void onClose(wxCloseEvent& event);
  void positionWidgets();
};
enum { DISPLAY_BLACK_OUT = 1 };

}  // namespace cszb_scoreboard
