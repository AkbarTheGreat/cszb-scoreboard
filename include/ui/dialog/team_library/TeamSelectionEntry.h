/*
ui/dialog/team_library/TeamSelectionEntry.h: A single entry in the team
selection box with all of the pertinent information for a given team. from the
team library or edit said library.

Copyright 2023-2025 Tracy Beck

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

#include <cstdint>  // for int32_t
#include <memory>   // for unique_ptr
#include <string>   // for string

#include "ui/widget/Button.h"       // for Button
#include "ui/widget/Label.h"        // for Label
#include "ui/widget/RadioButton.h"  // for RadioButton

namespace cszb_scoreboard {

class TeamSelectionBox;
class Panel;

namespace proto {
class TeamLibInfo;
enum TeamInfo_TeamType : int;
}  // namespace proto

class TeamSelectionEntry {
 public:
  static constexpr int32_t BORDER_SIZE = 3;

  TeamSelectionEntry(Panel* panel, TeamSelectionBox* owning_controller,
                     int32_t row, const proto::TeamLibInfo& team);
  void setName(const std::string& name);
  void teamSelectionChanged(proto::TeamInfo_TeamType team);
  auto isHome() -> bool { return home->selected(); }
  auto isAway() -> bool { return away->selected(); }

 private:
  TeamSelectionBox* parent;
  Panel* panel;
  int32_t index;

  std::unique_ptr<RadioButton> home;
  std::unique_ptr<RadioButton> away;
  std::unique_ptr<Label> name;
  std::unique_ptr<Label> default_team;
  std::unique_ptr<Button> remove;

  static auto teamLabel(proto::TeamInfo_TeamType type) -> std::string;

  void bindEvents();
  void positionWidgets();
  void homeButtonPressed();
  void awayButtonPressed();
  void removeButtonPressed();
  void nameClicked();
  void handleDefaultTeams(proto::TeamInfo_TeamType type);
};

}  // namespace cszb_scoreboard
