/*
ui/component/control/TeamSelector.h: A group of radio buttons which selects
which team's or teams' screen(s) to send data to.

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

#include "config.pb.h"

namespace cszb_scoreboard {

class TeamSelector : public wxRadioBox {
 public:
  explicit TeamSelector(wxWindow *parent);
  TeamSelector(wxWindow *parent, const proto::ScreenSide &initial_side);
  auto allSelected() -> bool;
  auto awaySelected() -> bool;
  auto homeSelected() -> bool;
};

}  // namespace cszb_scoreboard
