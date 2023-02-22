/*
ui/dialog/team_library/TeamSelectionBox.h: A box which lists all known teams and
information about them, with selections to chose home/away for the current
match.

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

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "team_library.pb.h"
#include "ui/dialog/team_library/TeamSelectionEntry.h"  // for TeamSelection...
#include "ui/widget/Panel.h"                            // for Panel
#include "ui/widget/ScrollingPanel.h"                   // for ScrollingPanel
#include "util/Singleton.h"                             // for Singleton

namespace cszb_scoreboard {

class TeamLibraryDialog;

namespace swx {
class Panel;
}  // namespace swx

class TeamSelectionBox : public Panel {
 public:
  TeamSelectionBox(swx::Panel* wx, TeamLibraryDialog* owning_controller)
      : TeamSelectionBox(wx, owning_controller, Singleton::getInstance()) {}

  void updateList();

  PUBLIC_TEST_ONLY
  TeamSelectionBox(swx::Panel* wx, TeamLibraryDialog* owning_controller,
                   Singleton* singleton);

 private:
  Singleton* singleton;
  TeamLibraryDialog* parent;
  proto::TeamLibrary library;

  std::vector<std::unique_ptr<TeamSelectionEntry>> team_selection_entries;
  std::unique_ptr<Panel> header;
  std::vector<std::unique_ptr<Label>> header_labels;
  std::unique_ptr<Panel> team_selection;
  std::unique_ptr<ScrollingPanel> team_selection_scrolling;

  void bindEvents();
  void positionWidgets();
  void createEntries();
  void createHeader();
};

}  // namespace cszb_scoreboard
