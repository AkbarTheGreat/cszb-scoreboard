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

MainView::MainView(const wxString& title, const wxPoint& pos,
                   const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
  FrameList::getInstance()->setMainView(this);

  createMenu();
  createStatusBar();

  preview_panel = new PreviewPanel(this);
  control_notebook = createControlNotebook();

  positionWidgets();
  bindEvents();
}

void MainView::createMenu() {
  wxMenu* menu_general = new wxMenu;
  menu_general->Append(GENERAL_SETTINGS, "&Settings",
                       "Configure the scoreboard");
  menu_general->AppendSeparator();
  menu_general->Append(wxID_EXIT);
  wxMenu* menu_display = new wxMenu;
  menu_display->Append(DISPLAY_BLACK_OUT, "&Black Out\tCtrl-B",
                       "Black out both screens");
  wxMenu* menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT);
  wxMenuBar* menu_bar = new wxMenuBar;
  menu_bar->Append(menu_general, "&General");
  menu_bar->Append(menu_display, "&Display");
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

wxNotebook* MainView::createControlNotebook() {
  wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
  text_entry = new TextEntry(preview_panel, notebook);
  notebook->AddPage(text_entry, "Text");
  wxStaticText* score_control =
      new wxStaticText(notebook, wxID_ANY, wxT("Coming Soon"));
  notebook->AddPage(score_control, "Score");
  return notebook;
}

void MainView::positionWidgets() {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  sizer->Add(preview_panel, 0, wxEXPAND | wxALL);
  sizer->Add(control_notebook, 0, wxEXPAND | wxALL);
  SetSizerAndFit(sizer);
}

void MainView::bindEvents() {
  Bind(wxEVT_CLOSE_WINDOW, &MainView::onClose, this);
  // Menu events bind against the frame itself, so a bare Bind() is useful
  // here.
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainView::showSettings, this,
       GENERAL_SETTINGS);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainView::onExit, this, wxID_EXIT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainView::onAbout, this, wxID_ABOUT);
}

void MainView::onExit(wxCommandEvent& event) { Close(true); }

void MainView::onAbout(wxCommandEvent& event) {
  wxMessageBox(
      "cszb-scoreboard: The ComedySportz Scoreboard.  Copyright (c) Tracy "
      "Beck, Licensed via the Apache License.",
      "About Scoreboard", wxOK | wxICON_INFORMATION);
}

void MainView::showSettings(wxCommandEvent& event) {
  settings_dialog = new SettingsDialog();
  settings_dialog->Create(this);
  settings_dialog->Show();
  settings_dialog->Bind(SETTINGS_UPDATED, &MainView::onSettingsChange, this);
}

void MainView::onSettingsChange(wxCommandEvent& event) {
  preview_panel->updatePreviewsFromSettings();
}

void MainView::onClose(wxCloseEvent& event) {
  FrameList::getInstance()->exitFrames();
  Destroy();
}

}  // namespace cszb_scoreboard
