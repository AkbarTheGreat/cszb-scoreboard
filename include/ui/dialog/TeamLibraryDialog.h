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

#include <map>     // for map
#include <memory>  // for unique_ptr

#include "ScoreboardCommon.h"     // for PUBLIC_TEST_ONLY
#include "config/ImageLibrary.h"  // for TemporaryImage...
#include "team_library.pb.h"
#include "ui/dialog/edit_image_library/FileListBox.h"  // for FileListBox
#include "ui/widget/Button.h"                          // for Button
#include "ui/widget/Divider.h"                         // for Divider
#include "ui/widget/Label.h"                           // for Label
#include "ui/widget/ListBox.h"                         // for ListBox
#include "ui/widget/Panel.h"                           // for Panel
#include "ui/widget/TabbedDialog.h"                    // for TabbedDialog
#include "ui/widget/Text.h"                            // for Text
#include "util/FilesystemPath.h"                       // for FilesystemPath
#include "util/Singleton.h"                            // for Singleton
#include "ui/component/control/ScoreControl.h"

class wxListEvent;

namespace cszb_scoreboard {
namespace swx {
class PropertySheetDialog;
}  // namespace swx

class ImageFromLibrary;

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
  std::unique_ptr<Label> name_label;
  std::unique_ptr<Text> name_entry;
  std::unique_ptr<Label> file_name_label;
  std::unique_ptr<Text> file_name_entry;

  void bindEvents();
  void positionWidgets();
  void saveSettings();
  static auto validateSettings() -> bool;
  void onOk();
  void onCancel();
};

}  // namespace cszb_scoreboard
