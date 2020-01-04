/*
ui/MainView.h: This class represents the main control window for the
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
#pragma once

#include <wx/wx.h>

#include <vector>

#include "ui/component/ControlPanel.h"
#include "ui/component/Menu.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/ScreenPreview.h"
#include "ui/component/control/TextEntry.h"
#include "ui/dialog/SettingsDialog.h"

namespace cszb_scoreboard {

class MainView : public wxFrame {
 public:
  MainView(const wxString& title, const wxPoint& pos, const wxSize& size);
  void setTextForPreview(wxString text, int font_size, proto::ScreenSide side);
  void updatePresenters(proto::ScreenSide side);

#ifdef SCOREBOARD_TESTING
  inline PreviewPanel* previewPanel() { return preview_panel; }
  TextEntry* textEntry() { return control_panel->textEntry(); }
#endif

 private:
  ControlPanel* control_panel;
  SettingsDialog* settings_dialog;
  PreviewPanel* preview_panel;
  void bindEvents();
  void createMenu();
  wxNotebook* createControlNotebook();
  void createStatusBar();
  void onExit(wxCommandEvent& event);
  void onAbout(wxCommandEvent& event);
  void onClose(wxCloseEvent& event);
  void onSettingsChange(wxCommandEvent& event);
  void positionWidgets();
  void showSettings(wxCommandEvent& event);
};

}  // namespace cszb_scoreboard
