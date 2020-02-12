/*
ui/component/control/LocalImage.cpp: Handles loading images from disk (or a
local library which is stored on disk) to display on one or both screens.

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

#include "ui/component/control/LocalImage.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const std::string IMAGE_SELECTION_STRING =
    "Image files (bmp, gif, jpeg, png)|*.bmp;*.gif;*.jpg;*.jpeg;*.png";

//  This string has a bunch of trailing whitespace to buffer out the
//  layout to make room for most filenames.  It's hacky, but it's a bit
//  better than completely resizing the panels, IMHO.
const std::string NO_IMAGE_MESSAGE =
    "<No Image Selected>                               ";

LocalImage *LocalImage::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  LocalImage *local_image = new LocalImage(preview_panel, parent);
  local_image->initializeWidgets();
  local_image->updatePreview();
  return local_image;
}

void LocalImage::createControls(wxPanel *control_panel) {
  inner_panel = new wxPanel(control_panel);
  browse_button = new wxButton(inner_panel, wxID_ANY, "Browse");
  screen_selection = new TeamSelector(inner_panel);
  current_file = new wxStaticText(control_panel, wxID_ANY, NO_IMAGE_MESSAGE);
  positionWidgets(control_panel);
  bindEvents();
}

void LocalImage::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *outer_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  outer_sizer->SetFlexibleDirection(wxBOTH);
  outer_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *inner_sizer = new wxFlexGridSizer(0, 2, 0, 0);
  inner_sizer->SetFlexibleDirection(wxBOTH);
  inner_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  inner_sizer->Add(browse_button, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);

  inner_panel->SetSizerAndFit(inner_sizer);

  outer_sizer->Add(current_file, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(inner_panel, 0, wxALL, 0);

  control_panel->SetSizerAndFit(outer_sizer);
}

void LocalImage::bindEvents() {
  browse_button->Bind(wxEVT_BUTTON, &LocalImage::browsePressed, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &LocalImage::screenChanged, this);
}

void LocalImage::updatePreview() {
  if (screen_selection->allSelected() && !all_screen_file.empty()) {
    // Send the image to both screens
    proto::ScreenSide side;
    side.set_home(true);
    side.set_away(true);
    previewPanel()->setTextForPreview("", 1, Color("Black"), false, side);
    previewPanel()->setImageForPreview(wxImage(all_screen_file.c_str()), side);
  } else {
    proto::ScreenSide home_side;
    home_side.set_home(true);
    proto::ScreenSide away_side;
    away_side.set_away(true);
    if (!home_screen_file.empty()) {
      previewPanel()->setTextForPreview("", 1, Color("Black"), false,
                                        home_side);
      previewPanel()->setImageForPreview(wxImage(home_screen_file.c_str()),
                                         home_side);
    }
    if (!away_screen_file.empty()) {
      previewPanel()->setTextForPreview("", 1, Color("Black"), false,
                                        away_side);
      previewPanel()->setImageForPreview(wxImage(away_screen_file.c_str()),
                                         away_side);
    }
  }
}

void LocalImage::browsePressed(wxCommandEvent &event) {
  wxFileDialog dialog(this, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() != wxID_CANCEL) {
    std::filesystem::path selected_file = (std::string)dialog.GetPath();
    if (screen_selection->allSelected()) {
      all_screen_file = selected_file;
    } else if (screen_selection->awaySelected()) {
      away_screen_file = selected_file;
    } else {
      home_screen_file = selected_file;
    }
    current_file->SetLabelText(selected_file.filename().c_str());
  }

  control_panel->Update();
  updatePreview();
}

void LocalImage::screenChanged(wxCommandEvent &event) {
  if (screen_selection->allSelected()) {
    if (all_screen_file.empty()) {
      current_file->SetLabelText(NO_IMAGE_MESSAGE);
    } else {
      current_file->SetLabelText(all_screen_file.filename().c_str());
    }
  } else if (screen_selection->homeSelected()) {
    if (home_screen_file.empty()) {
      current_file->SetLabelText(NO_IMAGE_MESSAGE);
    } else {
      current_file->SetLabelText(home_screen_file.filename().c_str());
    }
  } else if (screen_selection->awaySelected()) {
    if (away_screen_file.empty()) {
      current_file->SetLabelText(NO_IMAGE_MESSAGE);
    } else {
      current_file->SetLabelText(away_screen_file.filename().c_str());
    }
  }

  control_panel->Update();
  updatePreview();
}

}  // namespace cszb_scoreboard
