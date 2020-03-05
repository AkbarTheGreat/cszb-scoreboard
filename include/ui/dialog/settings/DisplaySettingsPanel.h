/*
ui/dialog/settings/DisplaySettingsPanel.h: The panel which contains display
settings.

Copyright 2019-2020 Tracy Beck

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

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "config/DisplayConfig.h"
#include "util/StringUtil.h"

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

}  // namespace cszb_scoreboard
