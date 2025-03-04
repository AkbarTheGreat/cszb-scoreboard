/*
ui/dialog/team_library/TeamSelectionBox.h: A box which lists all known teams and
information about them, with selections to chose home/away for the current
match.

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
#include <vector>   // for vector

#include "ScoreboardCommon.h"                           // for PUBLIC_TEST_ONLY
#include "team_library.pb.h"                            // for TeamLibInfo
#include "ui/dialog/team_library/TeamSelectionEntry.h"  // for TeamSelection...
#include "ui/widget/Label.h"                            // for Label
#include "ui/widget/Panel.h"                            // for Panel
#include "ui/widget/ScrollingPanel.h"                   // for ScrollingPanel
#include "util/Singleton.h"                             // for Singleton

namespace cszb_scoreboard {

class TeamLibraryDialog;
class FilesystemPath;
namespace proto {
enum TeamInfo_TeamType : int;
}  // namespace proto

namespace swx {
class Panel;
}  // namespace swx

class TeamSelectionBox : public Panel {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  TeamSelectionBox(swx::Panel* wx, TeamLibraryDialog* owning_controller)
      : TeamSelectionBox(wx, owning_controller, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP

  void updateList();
  void teamSelectedForSide(int32_t row, proto::TeamInfo_TeamType team);
  void teamSelectedForEdit(int32_t row);
  void addTeam(const std::string& name, const FilesystemPath& logo,
               proto::TeamInfo_TeamType type);
  void changeTeam(int32_t team_number, const std::string& name,
                  const FilesystemPath& logo, proto::TeamInfo_TeamType type);
  void deleteTeam(int32_t row_number);
  auto selectedTeams() -> proto::TeamLibraryDialogResponse;
  void saveLibrary();

  PUBLIC_TEST_ONLY
  TeamSelectionBox(swx::Panel* wx, TeamLibraryDialog* owning_controller,
                   Singleton* singleton);

 private:
  Singleton* singleton;
  TeamLibraryDialog* parent;
  proto::TeamLibrary library;
  std::string pad_text;

  std::vector<std::unique_ptr<TeamSelectionEntry>> team_selection_entries;
  std::unique_ptr<Panel> header;
  std::vector<std::unique_ptr<Label>> header_labels;
  std::unique_ptr<Panel> team_selection;
  std::unique_ptr<ScrollingPanel> team_selection_scrolling;

  void bindEvents();
  void positionWidgets();
  void clearOldEntries();
  void createEntries();
  void createHeader();
  auto placeholderTeam() -> proto::TeamLibInfo;
  void removeType(proto::TeamInfo_TeamType type);
  void setTeamInfo(proto::TeamLibInfo* team, const std::string& name,
                   const FilesystemPath& logo, proto::TeamInfo_TeamType type);
  static void copyTeam(const proto::TeamLibInfo& from, proto::TeamLibInfo* to);
};

}  // namespace cszb_scoreboard
