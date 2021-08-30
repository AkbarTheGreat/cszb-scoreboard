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

#include <wx/string.h>  // for operator+, wxString

#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"       // for SCOREBOARD_VERSION
#include "config/CommandArgs.h"     // for CommandArgs
#include "config/DisplayConfig.h"   // for DisplayConfig
#include "config/swx/defs.h"        // for wxID_ABOUT, wxID_EXIT, wxID_SEPAR...
#include "config/swx/event.h"       // for wxEVT_COMMAND_MENU_SELECTED, wxEV...
#include "ui/component/Menu.h"      // for DISPLAY_BLACK_OUT, GENERAL_SETTINGS
#include "ui/frame/FrameManager.h"  // for FrameManager
#include "ui/frame/HotkeyTable.h"   // for HotkeyTable
#include "ui/widget/PopUp.h"        // for PopUp
#include "util/StringUtil.h"        // for StringUtil

namespace cszb_scoreboard {
struct Position;
struct Size;

const int BORDER_SIZE = 0;

MainView::MainView(const std::string &title, const Position &pos,
                   const Size &size)
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
    update_timer = std::make_unique<UpdateTimer>(this);
  }

  // Set focus to the control_panel so that tab movement works correctly without
  // an initial click.
  control_panel->focus();
  HotkeyTable::getInstance()->installHotkeys(this);
}

void MainView::createMenu() {
  std::vector<MenuItem> general;
  general.emplace_back(MenuItem{.id = GENERAL_SETTINGS,
                                .name = "&Settings",
                                .description = "Configure the scoreboard"});
  general.emplace_back(MenuItem{.id = wxID_SEPARATOR});
  general.emplace_back(MenuItem{.id = wxID_EXIT});

  std::vector<MenuItem> display;
  display.emplace_back(MenuItem{.id = DISPLAY_BLACK_OUT,
                                .name = "&Black Out\tCtrl-B",
                                .description = "Black out both screens"});

  std::vector<MenuItem> help;
  help.emplace_back(MenuItem{.id = wxID_ABOUT});

  std::vector<MenuCategory> menu;
  menu.emplace_back(MenuCategory{.name = "&General", .items = general});
  menu.emplace_back(MenuCategory{.name = "&Display", .items = display});
  menu.emplace_back(MenuCategory{.name = "&Help", .items = help});

  menuBar(menu);
}

void MainView::positionWidgets() {
  addWidget(*preview_panel, 0, 0);
  addWidget(*control_panel, 1, 0);
  addWidgetWithSpan(*quick_state, 0, 1, 2, 1);
  runSizer();
}

void MainView::bindEvents() {
  bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent &event) -> void { onClose(); });
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void { this->showSettings(); },
      GENERAL_SETTINGS);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void { this->onExit(); }, wxID_EXIT);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [](wxCommandEvent &event) -> void { onAbout(); }, wxID_ABOUT);
  bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this](wxCommandEvent &event) -> void {
        this->preview_panel->blackout();
      },
      DISPLAY_BLACK_OUT);
}

void MainView::onExit() { closeWindow(); }

void MainView::onAbout() {
  std::string about_string =
      "cszb-scoreboard: The ComedySportz Scoreboard.  "
      "Version " SCOREBOARD_VERSION
      ".  Copyright (c) Tracy Beck, Licensed via the Apache License.";
  PopUp::Info("About Scoreboard", about_string);
}

void MainView::showSettings() {
  settings_dialog = std::make_unique<SettingsDialog>(
      childDialog("Scoreboard Settings"), this);
  settings_dialog->show();
  settings_dialog->bind(
      SETTINGS_UPDATED,
      [this](wxCommandEvent &event) -> void { this->onSettingsChange(); });
}

void MainView::onSettingsChange() {
  preview_panel->updatePreviewsFromSettings();
}

void MainView::onSettingsClose() {
  settings_dialog.reset();
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog.   So while we're at it, focus on the
  // control panel, to allow tab navigation to work without additional clicking.
  control_panel->focus();
}

void MainView::onClose() {
  // The following call deletes the pointer to this object, so should always be
  // done last.
  FrameManager::getInstance()->exitFrames();
}

}  // namespace cszb_scoreboard
