/*
ui/dialog/SettingsDialog.h: A settings dialog for configuring various aspects of
the scoreboard which are then saved as a part of the configuration protobuf.

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

#include <wx/clrpicker.h>
#include <wx/propdlg.h>
#include <wx/wx.h>

#include <vector>

#include "proto/config.pb.h"

namespace cszb_scoreboard {

class DisplaySettingsPanel : public wxPanel {
 public:
  DisplaySettingsPanel(wxPanel *parent, int display_number);
  proto::ScreenSide getSide();

 private:
  wxCheckBox *control_checkbox;
  wxCheckBox *home_checkbox;
  wxCheckBox *away_checkbox;
};

class TeamSettingsPanel : public wxPanel {
 public:
  TeamSettingsPanel(wxPanel *parent, int display_number);

 private:
  wxColourPickerCtrl *color_picker;
};

class SettingsDialog : public wxPropertySheetDialog {
 public:
  bool Create(wxWindow *parent);

 private:
  void bindEvents();
  wxPanel *createDisplayPage(wxBookCtrlBase *settings_book);
  wxPanel *createTeamsPage(wxBookCtrlBase *settings_book);
  void onOk(wxCommandEvent &event);
  void saveDisplaySettings();
  bool validateDisplaySettings();
  std::vector<DisplaySettingsPanel *> display_settings_panels;
  std::vector<TeamSettingsPanel *> team_settings_panels;
  wxWindow *parent;
};

}  // namespace cszb_scoreboard
