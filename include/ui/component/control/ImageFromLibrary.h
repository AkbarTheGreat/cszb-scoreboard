/*
ui/component/control/ImageFromLibrary.h: Handles loading images from a
pre-populated library of tagged images on disk and presenting them to one or
more screens.

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

#include <wx/srchctrl.h>
#include <wx/wx.h>

#include <vector>

#include "config.pb.h"
#include "ui/component/control/ImagePreview.h"
#include "ui/component/control/ScreenImageController.h"
#include "ui/dialog/EditImageLibraryDialog.h"

namespace cszb_scoreboard {

class ImageFromLibrary : public ScreenImageController {
 public:
  static ImageFromLibrary *Create(PreviewPanel *preview_panel,
                                  wxWindow *parent);

 private:
  int current_image_page = 0;
  wxButton *left_button, *right_button, *configure_button;
  wxSearchCtrl *search_box;
  wxPanel *main_panel, *search_panel, *image_preview_panel;
  wxStaticText *tag_list_label;
  std::vector<ImagePreview *> image_previews;
  std::vector<wxStaticText *> image_names;
  EditImageLibraryDialog *edit_dialog;

  ImageFromLibrary(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenImageController(preview_panel, parent) {}
  void bindEvents();
  void createControls(wxPanel *control_panel) override;
  void doSearch(wxCommandEvent &event);
  void positionWidgets(wxPanel *control_panel) override;
  void selectImage(wxMouseEvent &event);
  void setImages(wxString search, unsigned int page_number = 0);
  void editButton(wxCommandEvent &event);
  void pageChange(wxCommandEvent &event);
};

}  // namespace cszb_scoreboard