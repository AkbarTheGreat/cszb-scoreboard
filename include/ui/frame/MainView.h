/*
ui/frame/MainView.h: This class represents the main control window for the
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
#pragma once

#include <wx/wx.h>

#include <vector>

#include "ui/component/ControlPanel.h"
#include "ui/component/Menu.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/QuickStatePanel.h"
#include "ui/component/ScreenPreview.h"
#include "ui/component/control/TextEntry.h"
#include "ui/dialog/SettingsDialog.h"
#include "ui/event/UpdateTimer.h"

namespace cszb_scoreboard {

class MainView : public wxFrame {
 public:
  MainView(const wxString& title, const wxPoint& pos, const wxSize& size);
  auto controlPanel() -> ControlPanel* { return control_panel; }
  auto previewPanel() -> PreviewPanel* { return preview_panel; }

 private:
  void bindEvents();
  void createMenu();
  auto createControlNotebook() -> wxNotebook*;
  void createStatusBar();
  void positionWidgets();
  // wxWidgets callbacks, waive linting error for references.
  void onExit(wxCommandEvent& event);   // NOLINT(google-runtime-references)
  void onAbout(wxCommandEvent& event);  // NOLINT(google-runtime-references)
  void onClose(wxCloseEvent& event);    // NOLINT(google-runtime-references)
  void onSettingsChange(
      wxCommandEvent& event);  // NOLINT(google-runtime-references)
  void showSettings(
      wxCommandEvent& event);  // NOLINT(google-runtime-references)

  ControlPanel* control_panel;
  SettingsDialog* settings_dialog;
  PreviewPanel* preview_panel;
  QuickStatePanel* quick_state;
  UpdateTimer* update_timer;
};

}  // namespace cszb_scoreboard
