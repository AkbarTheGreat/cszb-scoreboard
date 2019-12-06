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

namespace cszb_scoreboard {

class MainView : public wxFrame {
 public:
  MainView(const wxString& title, const wxPoint& pos, const wxSize& size);

 private:
  std::vector<ScreenPreview*> screens;
  void blackout(wxCommandEvent& event);
  void onExit(wxCommandEvent& event);
  void onAbout(wxCommandEvent& event);
  void onClose(wxCloseEvent& event);
  wxDECLARE_EVENT_TABLE();
};
enum { FILE_BLACK_OUT = 1 };

}  // namespace cszb_scoreboard
