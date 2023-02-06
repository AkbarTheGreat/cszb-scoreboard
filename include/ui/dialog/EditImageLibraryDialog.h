/*
ui/dialog/EditImageLibraryDialog.h: A dialog which allows a user to
add/remove/edit images in the image library.

Copyright 2020-2023 Tracy Beck

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

#include "ScoreboardCommon.h"                          // for PUBLIC_TEST_ONLY
#include "config/ImageLibrary.h"                       // for TemporaryImage...
#include "image_library.pb.h"                          // for ImageInfo
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

class wxListEvent;

namespace cszb_scoreboard {
namespace swx {
class PropertySheetDialog;
}  // namespace swx

class ImageFromLibrary;

class EditImageLibraryDialog : public TabbedDialog {
 public:
  EditImageLibraryDialog(swx::PropertySheetDialog *wx, ImageFromLibrary *parent)
      : EditImageLibraryDialog(wx, parent, Singleton::getInstance()) {}

  PUBLIC_TEST_ONLY
  EditImageLibraryDialog(swx::PropertySheetDialog *wx, ImageFromLibrary *parent,
                         Singleton *singleton);

 private:
  std::unique_ptr<FileListBox> file_list;
  std::unique_ptr<Panel> box_panel;
  std::unique_ptr<Text> name_entry;
  std::unique_ptr<Label> name_label;
  std::unique_ptr<Label> full_name_label;
  std::unique_ptr<Text> full_name_entry;
  std::unique_ptr<Divider> root_divider;
  std::unique_ptr<Text> root_entry;
  std::unique_ptr<Button> root_browse;
  std::unique_ptr<Button> root_clear;
  std::unique_ptr<ListBox> tag_list;
  std::unique_ptr<TemporaryImageLibrary> library;
  std::map<FilesystemPath, proto::ImageInfo> images;
  ImageFromLibrary *parent;
  Singleton *singleton;

  void bindEvents();
  void positionWidgets();
  void saveSettings();
  static auto validateSettings() -> bool;
  void fileSelected(wxListEvent *event);
  void onOk();
  void onCancel();
  void fileUpdated(const FilesystemPath &prev, const FilesystemPath &curr);
  void nameUpdated();
  void refreshFiles();
  void rootBrowsePressed();
  void rootClearPressed();
  void tagDeleted(const wxListEvent &event);
  void tagsUpdated(const wxListEvent &event);
};

}  // namespace cszb_scoreboard
