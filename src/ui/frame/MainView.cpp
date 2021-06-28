/*
ui/frame/MainView.cpp: This class represents the main control window for the
application.

Copyright 2019-2021 Tracy Beck

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

#include <vector>

#include "config/CommandArgs.h"
#include "config/DisplayConfig.h"
#include "ui/UiUtil.h"
#include "ui/frame/FrameManager.h"
#include "ui/frame/HotkeyTable.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = 0;

MainView::MainView(const wxString& title, const wxPoint& pos,
                   const wxSize& size)
    : Frame(title, pos, size) {
  createMenu();
  setStatusBar("Welcome to ComedySportz Scoreboard, " +
               StringUtil::intToString(
                   DisplayConfig::getInstance()->numberOfDisplays()) +
               " displays found.");

  preview_panel = new PreviewPanel(this->wx);
  control_panel = new ControlPanel(this->wx, preview_panel);
  quick_state = new QuickStatePanel(this->wx);

  positionWidgets();
  bindEvents();

  if (CommandArgs::getInstance()->autoUpdate()) {
    update_timer = new UpdateTimer(this);
    update_timer->Start(1, true);
  }

  // Set focus to the control_panel so that tab movement works correctly without
  // an initial click.
  control_panel->SetFocus();
  HotkeyTable::getInstance()->installHotkeys(this->wx);
}

void MainView::createMenu() {
  auto* menu_general = new wxMenu;
  menu_general->Append(GENERAL_SETTINGS, "&Settings",
                       "Configure the scoreboard");
  menu_general->AppendSeparator();
  menu_general->Append(wxID_EXIT);
  auto* menu_display = new wxMenu;
  menu_display->Append(DISPLAY_BLACK_OUT, "&Black Out\tCtrl-B",
                       "Black out both screens");
  auto* menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT);

  std::vector<std::pair<wxMenu*, std::string>> menu;
  menu.emplace_back(std::pair<wxMenu*, std::string>{menu_general, "&General"});
  menu.emplace_back(std::pair<wxMenu*, std::string>{menu_display, "&Display"});
  menu.emplace_back(std::pair<wxMenu*, std::string>{menu_help, "&Help"});
  menuBar(menu);
}

void MainView::positionWidgets() {
  auto* sizer = new wxGridBagSizer();
  UiUtil::addToGridBag(sizer, preview_panel, 0, 0);
  UiUtil::addToGridBag(sizer, control_panel, 1, 0);
  UiUtil::addToGridBag(sizer, quick_state, 0, 1, 2, 1);
  setSizer(sizer);
}

void MainView::bindEvents() {
  bind(wxEVT_CLOSE_WINDOW,
       [this](wxCloseEvent& event) -> void { this->onClose(event); });
  // Menu events bind against the frame itself, so a bare Bind() is useful
  // here.
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent& event) -> void { this->showSettings(event); },
      GENERAL_SETTINGS);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent& event) -> void { this->onExit(event); },
      wxID_EXIT);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent& event) -> void { this->onAbout(event); },
      wxID_ABOUT);
}

void MainView::onExit(wxCommandEvent& event) { closeWindow(); }

// Callbacks cannot be static.
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MainView::onAbout(wxCommandEvent& event) {
  wxString about_string;
  about_string.Printf(
      "cszb-scoreboard: The ComedySportz Scoreboard.  Version %s.  Copyright "
      "(c) Tracy Beck, Licensed via the Apache License.",
      SCOREBOARD_VERSION);
  wxMessageBox(about_string, "About Scoreboard", wxOK | wxICON_INFORMATION);
}

void MainView::showSettings(wxCommandEvent& event) {
  settings_dialog = new SettingsDialog();
  settings_dialog->Create(this->wx);
  settings_dialog->Show();
  settings_dialog->Bind(
      SETTINGS_UPDATED,
      [this](wxCommandEvent& event) -> void { this->onSettingsChange(event); });
}

void MainView::onSettingsChange(wxCommandEvent& event) {
  preview_panel->updatePreviewsFromSettings();
}

void MainView::onClose(wxCloseEvent& event) {
  // The following call deletes the pointer to this object, so should always be
  // done last.
  FrameManager::getInstance()->exitFrames();
}

}  // namespace cszb_scoreboard
