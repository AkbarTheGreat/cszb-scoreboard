/*
ui/component/control/ImageFromLibrary.h: Handles loading images from a
pre-populated library of tagged images on disk and presenting them to one or
more screens.

Copyright 2019-2023 Tracy Beck

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
#include <string>  // for string
#include <vector>  // for vector

#include "ui/component/control/ImagePreview.h"           // for ImagePreview
#include "ui/component/control/ScreenImageController.h"  // for ScreenImageC...
#include "ui/dialog/EditImageLibraryDialog.h"
#include "ui/widget/Button.h"     // for Button
#include "ui/widget/Label.h"      // for Label
#include "ui/widget/Panel.h"      // for Panel
#include "ui/widget/SearchBox.h"  // for SearchBox

namespace cszb_scoreboard {

namespace swx {
class Panel;
}  // namespace swx

class ImageFromLibrary : public ScreenImageController {
 public:
  explicit ImageFromLibrary(swx::Panel *wx) : ScreenImageController(wx) {}
  static auto Create(swx::Panel *wx) -> std::unique_ptr<ImageFromLibrary>;
  void onEditDialogClose();

 private:
  int current_image_page = 0;
  std::unique_ptr<Button> left_button, right_button, configure_button;
  std::unique_ptr<SearchBox> search_box;
  std::unique_ptr<Panel> main_panel, search_panel, image_preview_panel;
  std::unique_ptr<Label> tag_list_label;
  std::vector<std::unique_ptr<ImagePreview>> image_previews;
  std::vector<std::unique_ptr<Label>> image_names;
  std::unique_ptr<EditImageLibraryDialog> edit_dialog;

  void bindEvents();
  void createControls(Panel *control_panel) override;
  void positionWidgets(Panel *control_panel) override;
  void setImages(const std::string &search, unsigned int page_number = 0);
  void doSearch();
  void selectImage(ImagePreview *image);
  void editButton();
  void pageChange(bool forward);
};

}  // namespace cszb_scoreboard
