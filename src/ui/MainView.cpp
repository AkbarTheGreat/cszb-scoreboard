/*
ui/MainView.cpp: This class represents the main control window for the
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
#include "ui/MainView.h"
#include "config/DisplayConfig.h"
#include "ui/FrameList.h"
#include "ui/ScreenSide.h"

namespace cszb_scoreboard {

const int BORDER_WIDTH = 10;

MainView::MainView(const wxString& title, const wxPoint& pos,
                   const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(FILE_BLACK_OUT, "&Black Out...\tCtrl-B",
                   "Black out both screens");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);
  CreateStatusBar();
  wxString statusText = "Welcome to ComedySportz Scoreboard, ";
  char* numBuffer = new char[16];
  statusText +=
      _itoa(DisplayConfig::getInstance()->NumberOfDisplays(), numBuffer, 10);
  delete[] numBuffer;
  statusText += " displays found.";
  SetStatusText(statusText);

  switch (DisplayConfig::getInstance()->NumberOfDisplays()) {
    default:
    case 0:
      // This feels like an impossible scenario, since you shouldn't be flat-out
      // headless, but treat it as one, for the purposes of just carrying on and
      // at least attempting to display the main window.
    case 1:
      // Only the main window, this is a deprecated case, and probably a
      // mistake.
      screens.push_back(new ScreenPreview(this, ScreenSide::SIDE_NONE));
	  #ifdef WXDEBUG
	  // Run fullscreen behind our control window in debugging circumstances
      screens.push_back(new ScreenPreview(this, ScreenSide::SIDE_SINGLE));
	  #endif
      break;
    case 2:
      // Single scoreboard display, split-screen
      screens.push_back(new ScreenPreview(this, ScreenSide::SIDE_SINGLE));
      break;
    case 3:
      // Two scoreboard displays
      screens.push_back(new ScreenPreview(this, ScreenSide::SIDE_LEFT));
      screens.push_back(new ScreenPreview(this, ScreenSide::SIDE_RIGHT));
      break;
  }
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 2, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto screen : screens) {
    sizer->Add(screen->widget(), 1, wxEXPAND | wxALL, BORDER_WIDTH);
  }
  SetSizerAndFit(sizer);
}
void MainView::OnExit(wxCommandEvent& event) { Close(true); }
void MainView::OnAbout(wxCommandEvent& event) {
  wxMessageBox(
      "cszb-scoreboard: The ComedySportz Scoreboard.  Copyright (c) Tracy "
      "Beck, Licensed via the Apache License.",
      "About Scoreboard", wxOK | wxICON_INFORMATION);
}
void MainView::Blackout(wxCommandEvent& event) {
  // TODO: Blackout screens here
}

void MainView::OnClose(wxCloseEvent& event) {
  wxLogDebug(wxT("Running On Close"));
  FrameList::getInstance()->exitFrames();
  Destroy();
}

/* Intellisense and auto-formatting both get very angry about our event table,
   so placing it at the end for convenience.
*/
// clang-format off
wxBEGIN_EVENT_TABLE(MainView, wxFrame)
EVT_MENU(FILE_BLACK_OUT, MainView::Blackout)
EVT_MENU(wxID_EXIT, MainView::OnExit)
EVT_MENU(wxID_ABOUT, MainView::OnAbout)
EVT_CLOSE(MainView::OnClose)
wxEND_EVENT_TABLE()
// clang-format on

}  // namespace cszb_scoreboard
