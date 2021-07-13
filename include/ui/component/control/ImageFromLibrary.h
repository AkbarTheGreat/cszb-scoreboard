/*
ui/component/control/ImageFromLibrary.h: Handles loading images from a
pre-populated library of tagged images on disk and presenting them to one or
more screens.

Copyright 2019-2021 Tracy Beck

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

#include <wx/string.h>                                   // for wxString
#include <vector>                                        // for vector
#include <memory>                                        // for unique_ptr

#include "ui/component/control/ImagePreview.h"           // for ImagePreview
#include "ui/component/control/ScreenImageController.h"  // for ScreenImageC...
#include "ui/widget/Button.h"                            // for Button
#include "ui/widget/Label.h"                             // for Label
#include "ui/widget/SearchBox.h"                         // for SearchBox
#include "ui/widget/Panel.h"                             // for Panel

namespace cszb_scoreboard {
class EditImageLibraryDialog;
class PreviewPanel;

namespace swx {
class Panel;
}  // namespace swx

class ImageFromLibrary : public ScreenImageController {
 public:
  ImageFromLibrary(PreviewPanel *preview_panel, swx::Panel *wx)
      : ScreenImageController(preview_panel, wx) {}
  static auto Create(PreviewPanel *preview_panel, swx::Panel *wx)
      -> std::unique_ptr<ImageFromLibrary>;

 private:
  int current_image_page = 0;
  std::unique_ptr<Button> left_button, right_button, configure_button;
  std::unique_ptr<SearchBox> search_box;
  std::unique_ptr<Panel> main_panel, search_panel, image_preview_panel;
  std::unique_ptr<Label> tag_list_label;
  std::vector<std::unique_ptr<ImagePreview>> image_previews;
  std::vector<std::unique_ptr<Label>> image_names;
  // All of the following pointer types are references to wxWidgets elements
  // that wxWidgets maintains.  They are raw pointers as wxWidgets will handle
  // destruction of them at shutdown.
  EditImageLibraryDialog *edit_dialog;

  void bindEvents();
  void createControls(Panel *control_panel) override;
  void positionWidgets(Panel *control_panel) override;
  void setImages(const wxString &search, unsigned int page_number = 0);
  void doSearch();
  void selectImage(const ImagePreview &image);
  void editButton();
  void pageChange(bool forward);
};

}  // namespace cszb_scoreboard
