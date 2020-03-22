/*
ui/component/control/ImageLibrary.cpp: Handles loading images from a
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

#include "ui/component/control/ImageLibrary.h"

#include <filesystem>

#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ImageLibrary *ImageLibrary::Create(PreviewPanel *preview_panel,
                                   wxWindow *parent) {
  ImageLibrary *library = new ImageLibrary(preview_panel, parent);
  library->initializeWidgets();
  library->updatePreview();
  return library;
}

void ImageLibrary::createControls(wxPanel *control_panel) {
  ScreenImageController::createControls(control_panel);
  search_box = new wxSearchCtrl(control_panel, wxID_ANY);
  search_box->SetDescriptiveText("Find by tag/name");
  search_box->ShowSearchButton(false);
  search_box->ShowCancelButton(true);
  positionWidgets(control_panel);
  bindEvents();
}

void ImageLibrary::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 1, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  sizer->Add(current_image_label, 0, wxALL, BORDER_SIZE);
  sizer->Add(search_box, 0, wxALL, BORDER_SIZE);
  sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);

  control_panel->SetSizerAndFit(sizer);
}

void ImageLibrary::bindEvents() {}

}  // namespace cszb_scoreboard
