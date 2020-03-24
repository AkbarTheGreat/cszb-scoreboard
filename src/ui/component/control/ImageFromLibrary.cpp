/*
ui/component/control/ImageFromLibrary.cpp: Handles loading images from a
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

#include "ui/component/control/ImageFromLibrary.h"

#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const int NUM_PREVIEWS = 5;
const wxSize PREVIEW_SIZE(160, 90);

ImageFromLibrary *ImageFromLibrary::Create(PreviewPanel *preview_panel,
                                   wxWindow *parent) {
  ImageFromLibrary *library = new ImageFromLibrary(preview_panel, parent);
  library->initializeWidgets();
  library->updatePreview();
  return library;
}

void ImageFromLibrary::createControls(wxPanel *control_panel) {
  ScreenImageController::createControls(control_panel);
  search_panel = new wxPanel(control_panel);
  image_preview_panel = new wxPanel(control_panel);

  // Reparent our screen_selection to position it into inner_panel, for layout.
  screen_selection->Reparent(search_panel);

  search_box = new wxSearchCtrl(search_panel, wxID_ANY);
  search_box->SetDescriptiveText("Find by tag/name");
  search_box->ShowSearchButton(false);
  search_box->ShowCancelButton(true);

  left_button = new wxButton(search_panel, wxID_ANY, "<");
  right_button = new wxButton(search_panel, wxID_ANY, ">");
  configure_button = new wxButton(control_panel, wxID_ANY, "Edit Library");

  for (int i = 0; i < NUM_PREVIEWS; i++) {
    // TODO: Populate this with images from an actual library.
    ImagePreview *preview = new ImagePreview(image_preview_panel, PREVIEW_SIZE,
                                             "C:/temp/sample_image.jpg");
    image_previews.push_back(preview);
  }

  positionWidgets(control_panel);
  bindEvents();
}

void ImageFromLibrary::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *main_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  main_sizer->SetFlexibleDirection(wxBOTH);
  main_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *search_panel_sizer = new wxFlexGridSizer(0, 2, 0, 0);
  search_panel_sizer->SetFlexibleDirection(wxBOTH);
  search_panel_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *image_preview_sizer =
      new wxFlexGridSizer(0, NUM_PREVIEWS, 0, 0);
  image_preview_sizer->SetFlexibleDirection(wxBOTH);
  image_preview_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  search_panel_sizer->Add(search_box, 0, wxALL, BORDER_SIZE);
  search_panel_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);
  search_panel_sizer->Add(left_button, 0, wxALL, BORDER_SIZE);
  search_panel_sizer->Add(right_button, 0, wxALL, BORDER_SIZE);

  main_sizer->Add(current_image_label, 0, wxALL, BORDER_SIZE);
  main_sizer->Add(search_panel, 0, wxALL, BORDER_SIZE);
  main_sizer->Add(image_preview_panel, 0, wxALL, BORDER_SIZE);
  main_sizer->Add(configure_button, 0, wxALL, BORDER_SIZE);

  for (auto preview : image_previews) {
    image_preview_sizer->Add(preview, 0, wxALL, BORDER_SIZE);
  }

  search_panel->SetSizerAndFit(search_panel_sizer);
  image_preview_panel->SetSizerAndFit(image_preview_sizer);
  control_panel->SetSizerAndFit(main_sizer);
}

void ImageFromLibrary::bindEvents() {}

}  // namespace cszb_scoreboard
