/*
ui/frame/MainView.h: This class represents the main control window for the
application.

Copyright 2019-2024 Tracy Beck

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

#include <wx/defs.h>    // for wxID_ANY
#include <wx/string.h>  // for wxString

#include <memory>  // for unique_ptr
#include <string>  // for string

#include "ScoreboardCommon.h"              // for PUBLIC_TEST_ONLY
#include "config/Position.h"               // for Position, Size
#include "ui/component/ControlPanel.h"     // for ControlPanel
#include "ui/component/PreviewPanel.h"     // for PreviewPanel
#include "ui/component/QuickStatePanel.h"  // for QuickStatePanel
#include "ui/dialog/SettingsDialog.h"      // for SettingsDialog
#include "ui/event/UpdateTimer.h"          // for UpdateTimer
#include "ui/widget/Frame.h"               // for Frame
#include "ui/widget/swx/Frame.h"           // for FrameImpl, Frame (ptr only)
#include "util/Singleton.h"                // for Singleton

class wxNotebook;

namespace cszb_scoreboard {

class ScreenText;

class MainView : public Frame {
 public:
  MainView(const std::string &title, const Position &pos, const Size &size)
      : MainView(new swx::FrameImpl(nullptr, wxID_ANY, title, pos.toWx(),
                                    size.toWx()),
                 Singleton::getInstance()) {}
  void init();
  auto controlPanel() -> ControlPanel * { return control_panel.get(); }
  auto previewPanel() -> PreviewPanel * { return preview_panel.get(); }
  void resetDisplays(bool is_initial = false);
  auto scoreQuickState() -> ScreenText * {
    return quick_state ? quick_state->scorePanel() : nullptr;
  }
  void onSettingsClose();
  virtual void refreshPreviews() {
    preview_panel->refresh();
    quick_state->refresh();
  }

  PUBLIC_TEST_ONLY
  MainView(swx::Frame *wx, Singleton *singleton);

 private:
  void bindEvents();
  void createMenu();
  auto createControlNotebook() -> wxNotebook *;
  void createStatusBar();
  void positionWidgets();
  void onExit();
  static void onAbout();
  void onClose();
  void onSettingsChange();
  void showSettings();

  std::unique_ptr<ControlPanel> control_panel;
  std::unique_ptr<SettingsDialog> settings_dialog;
  std::unique_ptr<PreviewPanel> preview_panel;
  std::unique_ptr<QuickStatePanel> quick_state;
  std::unique_ptr<UpdateTimer> update_timer;
  Singleton *singleton;
};

}  // namespace cszb_scoreboard
