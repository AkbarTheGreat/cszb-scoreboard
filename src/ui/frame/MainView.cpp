/*
ui/frame/MainView.cpp: This class represents the main control window for the
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
#include "ui/frame/MainView.h"

#include "config/DisplayConfig.h"
#include "ui/frame/FrameList.h"
#include "ui/frame/HotkeyTable.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

MainView::MainView(const wxString& title, const wxPoint& pos,
                   const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
  FrameList::getInstance()->setMainView(this);

  createMenu();
  createStatusBar();

  preview_panel = new PreviewPanel(this);
  control_panel = new ControlPanel(this, preview_panel);

  positionWidgets();
  bindEvents();

  update_timer = new UpdateTimer(this);
  update_timer->Start(0, true);

  HotkeyTable::getInstance()->installHotkeys(this);
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
  status_text +=
      StringUtil::intToString(DisplayConfig::getInstance()->numberOfDisplays());
  status_text += " displays found.";
  SetStatusText(status_text);
}

void MainView::positionWidgets() {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  sizer->Add(preview_panel, 0, wxEXPAND | wxALL);
  sizer->Add(control_panel, 0, wxEXPAND | wxALL);
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
