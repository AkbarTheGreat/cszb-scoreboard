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

#include <wx/defs.h>    // for wxID_ABOUT, wxID_EXIT, wxICON_...
#include <wx/event.h>   // for wxCommandEvent (ptr only), wxE...
#include <wx/menu.h>    // IWYU pragma: keep for wxMenu
#include <wx/msgdlg.h>  // for wxMessageBox

#include <string>   // for string
#include <utility>  // for pair
#include <vector>   // for vector

#include "ScoreboardCommon.h"          // for SCOREBOARD_VERSION
#include "config/CommandArgs.h"        // for CommandArgs
#include "config/DisplayConfig.h"      // for DisplayConfig
#include "ui/component/Menu.h"         // for DISPLAY_BLACK_OUT, GENERAL_SET...
#include "ui/dialog/SettingsDialog.h"  // for SettingsDialog, SETTINGS_UPDATED
#include "ui/event/UpdateTimer.h"      // for UpdateTimer
#include "ui/frame/FrameManager.h"     // for FrameManager
#include "ui/frame/HotkeyTable.h"      // for HotkeyTable
#include "util/StringUtil.h"           // for StringUtil
// IWYU pragma: no_include <wx/gtk/menu.h>

namespace cszb_scoreboard {

const int BORDER_SIZE = 0;

MainView::MainView(const wxString &title, const wxPoint &pos,
                   const wxSize &size)
    : Frame(title, pos, size) {
  createMenu();
  setStatusBar("Welcome to ComedySportz Scoreboard, " +
               StringUtil::intToString(
                   DisplayConfig::getInstance()->numberOfDisplays()) +
               " displays found.");

  preview_panel = std::make_unique<PreviewPanel>(childPanel());
  control_panel =
      std::make_unique<ControlPanel>(childNotebook(), preview_panel.get());
  quick_state = std::make_unique<QuickStatePanel>(childPanel());

  positionWidgets();
  bindEvents();

  if (CommandArgs::getInstance()->autoUpdate()) {
    update_timer = new UpdateTimer(this);
    update_timer->Start(1, true);
  }

  // Set focus to the control_panel so that tab movement works correctly without
  // an initial click.
  control_panel->focus();
  HotkeyTable::getInstance()->installHotkeys(this);
}

void MainView::createMenu() {
  auto *menu_general = new wxMenu;
  menu_general->Append(GENERAL_SETTINGS, "&Settings",
                       "Configure the scoreboard");
  menu_general->AppendSeparator();
  menu_general->Append(wxID_EXIT);
  auto *menu_display = new wxMenu;
  menu_display->Append(DISPLAY_BLACK_OUT, "&Black Out\tCtrl-B",
                       "Black out both screens");
  auto *menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT);

  std::vector<std::pair<wxMenu *, std::string>> menu;
  menu.emplace_back(std::pair<wxMenu *, std::string>{menu_general, "&General"});
  menu.emplace_back(std::pair<wxMenu *, std::string>{menu_display, "&Display"});
  menu.emplace_back(std::pair<wxMenu *, std::string>{menu_help, "&Help"});
  menuBar(menu);
}

void MainView::positionWidgets() {
  addWidget(*preview_panel, 0, 0);
  addWidget(*control_panel, 1, 0);
  addWidgetWithSpan(*quick_state, 0, 1, 2, 1);
  runSizer();
}

void MainView::bindEvents() {
  bind(wxEVT_CLOSE_WINDOW,
       [this](wxCloseEvent &event) -> void { this->onClose(event); });
  // Menu events bind against the frame itself, so a bare Bind() is useful
  // here.
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void { this->showSettings(event); },
      GENERAL_SETTINGS);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void { this->onExit(event); },
      wxID_EXIT);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void { this->onAbout(event); },
      wxID_ABOUT);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void {
        this->preview_panel->blackout();
      },
      DISPLAY_BLACK_OUT);
}

void MainView::onExit(wxCommandEvent &event) { closeWindow(); }

// Callbacks cannot be static.
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MainView::onAbout(wxCommandEvent &event) {
  wxString about_string;
  about_string.Printf(
      "cszb-scoreboard: The ComedySportz Scoreboard.  Version %s.  Copyright "
      "(c) Tracy Beck, Licensed via the Apache License.",
      SCOREBOARD_VERSION);
  wxMessageBox(about_string, "About Scoreboard", wxOK | wxICON_INFORMATION);
}

void MainView::showSettings(wxCommandEvent &event) {
  settings_dialog =
      new SettingsDialog(childDialog("Scoreboard Settings"), this);
  settings_dialog->show();
  settings_dialog->bind(
      SETTINGS_UPDATED,
      [this](wxCommandEvent &event) -> void { this->onSettingsChange(event); });
}

void MainView::onSettingsChange(wxCommandEvent &event) {
  preview_panel->updatePreviewsFromSettings();
}

void MainView::onClose(wxCloseEvent &event) {
  // The following call deletes the pointer to this object, so should always be
  // done last.
  FrameManager::getInstance()->exitFrames();
}

}  // namespace cszb_scoreboard
