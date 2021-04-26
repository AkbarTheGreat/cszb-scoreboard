/*
ui/dialog/settings/TeamSettingsPanel.h: The panel which contains team
settings.

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

#include <wx/clrpicker.h>
#include <wx/wx.h>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "config/TeamConfig.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

class TeamSettingsPanel : public wxPanel {
 public:
  TeamSettingsPanel(wxPanel *parent, int team_index,
                    proto::TeamInfo_TeamType team);
  void copyFrom(TeamSettingsPanel *other);
  auto teamColor() -> Color;
  auto team() -> proto::TeamInfo_TeamType { return team_type; }

 private:
  int index;
  proto::TeamInfo_TeamType team_type;
  wxStaticText *label;
  wxColourPickerCtrl *color_picker;
  wxPanel *button_panel;
  wxButton *down_button;
  wxButton *up_button;
  void createButtonPanel();
  void moveTeam(wxCommandEvent &event);  // NOLINT(google-runtime-references)
                                         // wxWidgets callback.
};

}  // namespace cszb_scoreboard
