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

#include <wx/gdicmn.h>  // for wxPoint, wxSize (ptr only)
#include <wx/string.h>  // for wxString

#include <memory>  // for unique_ptr

#include "ui/component/ControlPanel.h"     // for ControlPanel
#include "ui/component/PreviewPanel.h"     // for PreviewPanel
#include "ui/component/QuickStatePanel.h"  // for QuickStatePanel
#include "ui/dialog/SettingsDialog.h"  // for SettingsDialog, SETTINGS_UPDATED
#include "ui/event/UpdateTimer.h"      // for UpdateTimer
#include "ui/widget/Frame.h"           // for Frame

class wxCloseEvent;
class wxCommandEvent;
class wxNotebook;

namespace cszb_scoreboard {

class MainView : public Frame {
 public:
  MainView(const wxString &title, const wxPoint &pos, const wxSize &size);
  auto controlPanel() -> ControlPanel * { return control_panel.get(); }
  auto previewPanel() -> PreviewPanel * { return preview_panel.get(); }

 private:
  void bindEvents();
  void createMenu();
  auto createControlNotebook() -> wxNotebook *;
  void createStatusBar();
  void positionWidgets();
  // wxWidgets callbacks, waive linting error for references.
  void onExit(wxCommandEvent &event);   // NOLINT(google-runtime-references)
  void onAbout(wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void onClose(wxCloseEvent &event);    // NOLINT(google-runtime-references)
  void onSettingsChange(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void showSettings(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)

  std::unique_ptr<ControlPanel> control_panel;
  std::unique_ptr<SettingsDialog> settings_dialog;
  std::unique_ptr<PreviewPanel> preview_panel;
  std::unique_ptr<QuickStatePanel> quick_state;
  std::unique_ptr<UpdateTimer> update_timer;
};

}  // namespace cszb_scoreboard
