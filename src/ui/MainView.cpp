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

namespace cszb_scoreboard {

const int BORDER_WIDTH = 10;

MainView::MainView(const wxString& title, const wxPoint& pos,
                   const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
  FrameList::getInstance()->setMainView(this);

  createMenu();
  createStatusBar();
  createPreviews();

  text_entry = new TextEntry(this);

  positionWidgets();
  bindEvents();
}

void MainView::createMenu() {
  wxMenu* menu_file = new wxMenu;
  wxMenuItem* blackout_option = menu_file->Append(
      FILE_BLACK_OUT, "&Black Out...\tCtrl-B", "Black out both screens");
  menu_file->AppendSeparator();
  menu_file->Append(wxID_EXIT);
  wxMenu* menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT);
  wxMenuBar* menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
}

void MainView::createStatusBar() {
  CreateStatusBar();
  wxString status_text = "Welcome to ComedySportz Scoreboard, ";
  char* num_buffer = new char[16];
  status_text +=
      _itoa(DisplayConfig::getInstance()->numberOfDisplays(), num_buffer, 10);
  delete[] num_buffer;
  status_text += " displays found.";
  SetStatusText(status_text);
}

void MainView::createPreviews() {
  for (int i = 0; i < DisplayConfig::getInstance()->numberOfDisplays(); ++i) {
    proto::DisplayInfo display_info =
        DisplayConfig::getInstance()->displayDetails(i);
    if (display_info.side().error() || display_info.side().home() ||
        display_info.side().away()) {
      screens.push_back(new ScreenPreview(this, display_info.side(), i));
    }
  }
}

void MainView::positionWidgets() {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 2, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  for (auto screen : screens) {
    sizer->Add(screen->widget(), 1, wxEXPAND | wxALL, BORDER_WIDTH);
  }
  sizer->Add(text_entry, 1, wxEXPAND | wxALL, BORDER_WIDTH);
  SetSizerAndFit(sizer);
}

void MainView::bindEvents() {
  Bind(wxEVT_CLOSE_WINDOW, &MainView::onClose, this);
  // Menu events bind against the frame itself, so a bare Bind() is useful
  // here.
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainView::blackout, this, FILE_BLACK_OUT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainView::onExit, this, wxID_EXIT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainView::onAbout, this, wxID_ABOUT);
}

void MainView::setTextForPreview(wxString text, int font_size,
                                 proto::ScreenSide side) {
  for (auto preview : screens) {
    ScreenText* screen_text = preview->widget();
    screen_text->setText(text, font_size, side);
    screen_text->Refresh();
  }
}

void MainView::updatePresenters(proto::ScreenSide side) {
  for (auto preview : screens) {
    preview->sendToPresenter(side);
  }
}

void MainView::updatePresenters() {
  for (auto preview : screens) {
    preview->sendToPresenter();
  }
}

void MainView::onExit(wxCommandEvent& event) { Close(true); }

void MainView::onAbout(wxCommandEvent& event) {
  wxMessageBox(
      "cszb-scoreboard: The ComedySportz Scoreboard.  Copyright (c) Tracy "
      "Beck, Licensed via the Apache License.",
      "About Scoreboard", wxOK | wxICON_INFORMATION);
}

void MainView::blackout(wxCommandEvent& event) {
  // TODO: Blackout screens here
}

void MainView::onClose(wxCloseEvent& event) {
  wxLogDebug(wxT("Running On Close"));
  FrameList::getInstance()->exitFrames();
  Destroy();
}

}  // namespace cszb_scoreboard
