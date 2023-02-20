/*
ui/dialog/TeamLibraryDialog.h: A dialog which allows a user to select a team
from the team library or edit said library.

Copyright 2023 Tracy Beck

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

#include <memory>  // for unique_ptr
#include <vector>  // for vector

#include "ScoreboardCommon.h"                           // for PUBLIC_TEST_ONLY
#include "team_library.pb.h"                            // for TeamLibrary
#include "ui/dialog/team_library/TeamSelectionEntry.h"  // for TeamSelection...
#include "ui/widget/Button.h"                           // for Button
#include "ui/widget/Divider.h"                          // for Divider
#include "ui/widget/Label.h"                            // for Label
#include "ui/widget/Panel.h"                            // for Panel
#include "ui/widget/ScrollingPanel.h"                   // for ScrollingPanel
#include "ui/widget/TabbedDialog.h"                     // for TabbedDialog
#include "ui/widget/Text.h"                             // for Text
#include "util/Singleton.h"                             // for Singleton

namespace cszb_scoreboard {
class ScoreControl;

namespace swx {
class PropertySheetDialog;
}  // namespace swx

class TeamLibraryDialog : public TabbedDialog {
 public:
  TeamLibraryDialog(swx::PropertySheetDialog *wx, ScoreControl *parent)
      : TeamLibraryDialog(wx, parent, Singleton::getInstance()) {}

  PUBLIC_TEST_ONLY
  TeamLibraryDialog(swx::PropertySheetDialog *wx, ScoreControl *parent,
                    Singleton *singleton);

 private:
  proto::TeamLibrary library;
  Singleton *singleton;
  ScoreControl *parent;

  // UI elements
  std::unique_ptr<Panel> box_panel;
  std::unique_ptr<Panel> bottom_panel;
  std::unique_ptr<Panel> team_selection;
  std::unique_ptr<ScrollingPanel> team_selection_scrolling;
  std::vector<std::unique_ptr<TeamSelectionEntry>> team_selection_entries;
  std::unique_ptr<Divider> divider;
  std::unique_ptr<Label> name_label;
  std::unique_ptr<Text> name_entry;
  std::unique_ptr<Label> file_name_label;
  std::unique_ptr<Text> file_name_entry;
  std::unique_ptr<Label> default_team_label;
  std::unique_ptr<Label> default_team_selector;
  std::unique_ptr<Button> add_update_button;

  void bindEvents();
  void positionWidgets();
  void populateTeamSelection();
  void saveSettings();
  static auto validateSettings() -> bool;
  void onOk();
  void onCancel();
};

}  // namespace cszb_scoreboard
