/*
ui/dialog/TeamLibraryDialog.h: A dialog which allows a user to select a team
from the team library or edit said library.

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

#include "ScoreboardCommon.h"                         // for PUBLIC_TEST_ONLY
#include "ui/dialog/team_library/TeamSelectionBox.h"  // for TeamSelectionBox
#include "ui/widget/Button.h"                         // for Button
#include "ui/widget/Divider.h"                        // for Divider
#include "ui/widget/DropDown.h"                       // for DropDown
#include "ui/widget/Label.h"                          // for Label
#include "ui/widget/Panel.h"                          // for Panel
#include "ui/widget/TabbedDialog.h"                   // for TabbedDialog
#include "ui/widget/Text.h"                           // for Text
#include "util/Singleton.h"                           // for Singleton

namespace cszb_scoreboard {
class ScoreControl;
class FilesystemPath;
namespace proto {
enum TeamInfo_TeamType : int;
}  // namespace proto

namespace swx {
class PropertySheetDialog;
}  // namespace swx

class TeamLibraryDialog : public TabbedDialog {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  TeamLibraryDialog(swx::PropertySheetDialog *wx, ScoreControl *parent)
      : TeamLibraryDialog(wx, parent, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP

  void clearEdit();
  void editTeam(int32_t row_number, const std::string &name,
                const FilesystemPath &logo, proto::TeamInfo_TeamType type);

  PUBLIC_TEST_ONLY
  TeamLibraryDialog(swx::PropertySheetDialog *wx, ScoreControl *parent,
                    Singleton *singleton);

 private:
  Singleton *singleton;
  ScoreControl *parent;
  int32_t row_for_edit;

  // UI elements
  std::unique_ptr<Panel> box_panel;
  std::unique_ptr<TeamSelectionBox> team_selection;
  std::unique_ptr<Panel> bottom_panel;
  std::unique_ptr<Divider> divider;
  std::unique_ptr<Label> name_label;
  std::unique_ptr<Text> name_entry;
  std::unique_ptr<Label> file_name_label;
  std::unique_ptr<Label> file_name_entry;
  std::unique_ptr<Label> default_team_label;
  std::unique_ptr<DropDown> default_team_selector;
  std::unique_ptr<Button> add_update_button;
  std::unique_ptr<Button> clear_logo_button;

  void bindEvents();
  void positionWidgets();
  void saveSettings();
  static auto validateSettings() -> bool;
  void onOk();
  void onCancel();
  void onAddOrUpdate();
  void pickLogo();
  void updateScoreTeams();
};

}  // namespace cszb_scoreboard
