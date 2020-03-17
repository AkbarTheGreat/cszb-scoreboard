/*
ui/component/control/LocalImage.cpp: Handles loading images from disk or from
the clipboard.

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

#include <wx/clipbrd.h>

#include <filesystem>

#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const std::string IMAGE_SELECTION_STRING =
    "Image files (bmp, gif, jpeg, png)|*.bmp;*.gif;*.jpg;*.jpeg;*.png";

//  These strings have a bunch of trailing whitespace to buffer out the
//  layout to make room for most filenames.  It's hacky, but it's a bit
//  better than completely resizing the panels, IMHO.
const std::string NO_IMAGE_MESSAGE =
    "<No Image Selected>                               ";

const std::string CLIPBOARD_IMAGE_MESSAGE =
    "<Image Loaded From Clipboard>                     ";

LocalImage *LocalImage::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  LocalImage *local_image = new LocalImage(preview_panel, parent);
  local_image->initializeWidgets();
  local_image->updatePreview();
  return local_image;
}

void LocalImage::createControls(wxPanel *control_panel) {
  inner_panel = new wxPanel(control_panel);
  button_panel = new wxPanel(inner_panel);
  browse_button = new wxButton(button_panel, wxID_ANY, "Browse");
  paste_button = new wxButton(button_panel, wxID_ANY, "Load From Clipboard");
  screen_selection = new TeamSelector(inner_panel, ProtoUtil::allSide());
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

  wxFlexGridSizer *button_sizer = new wxFlexGridSizer(2, 0, 0, 0);
  inner_sizer->SetFlexibleDirection(wxBOTH);
  inner_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  button_sizer->Add(browse_button, 0, wxALL, BORDER_SIZE);
  button_sizer->Add(paste_button, 0, wxALL, BORDER_SIZE);

  button_panel->SetSizerAndFit(button_sizer);

  inner_sizer->Add(button_panel, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);

  inner_panel->SetSizerAndFit(inner_sizer);

  outer_sizer->Add(current_file, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(inner_panel, 0, wxALL, 0);

  control_panel->SetSizerAndFit(outer_sizer);
}

void LocalImage::bindEvents() {
  browse_button->Bind(wxEVT_BUTTON, &LocalImage::browsePressed, this);
  paste_button->Bind(wxEVT_BUTTON, &LocalImage::pastePressed, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &LocalImage::screenChanged, this);
}

void LocalImage::updatePreview() {
  if (screen_selection->allSelected() && all_screen_image.IsOk()) {
    // Send the image to both screens
    previewPanel()->setTextForPreview("", 1, Color("Black"), false,
                                      ProtoUtil::allSide());
    previewPanel()->setImageForPreview(all_screen_image, ProtoUtil::allSide());
  } else {
    if (home_screen_image.IsOk()) {
      previewPanel()->setTextForPreview("", 1, Color("Black"), false,
                                        ProtoUtil::homeSide());
      previewPanel()->setImageForPreview(home_screen_image,
                                         ProtoUtil::homeSide());
    }
    if (away_screen_image.IsOk()) {
      previewPanel()->setTextForPreview("", 1, Color("Black"), false,
                                        ProtoUtil::awaySide());
      previewPanel()->setImageForPreview(away_screen_image,
                                         ProtoUtil::awaySide());
    }
  }
}  // namespace cszb_scoreboard

void LocalImage::browsePressed(wxCommandEvent &event) {
  wxFileDialog dialog(this, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() != wxID_CANCEL) {
    std::filesystem::path selected_file = (std::string)dialog.GetPath();
    if (screen_selection->allSelected()) {
      all_screen_image = wxImage(selected_file.c_str());
      all_screen_filename = selected_file.filename().string();
    } else if (screen_selection->awaySelected()) {
      away_screen_image = wxImage(selected_file.c_str());
      away_screen_filename = selected_file.filename().string();
    } else {
      home_screen_image = wxImage(selected_file.c_str());
      home_screen_filename = selected_file.filename().string();
    }
    current_file->SetLabelText(selected_file.filename().c_str());
  }

  control_panel->Update();
  updatePreview();
}

void LocalImage::pastePressed(wxCommandEvent &event) {
  wxImage clipboard_image;

  if (wxTheClipboard->Open()) {
    if (wxTheClipboard->IsSupported(wxDF_BITMAP)) {
      wxBitmapDataObject data;
      wxTheClipboard->GetData(data);
      wxTheClipboard->Close();
      clipboard_image = data.GetBitmap().ConvertToImage();
    } else {
      wxTheClipboard->Close();
      wxMessageBox("ERROR: No supported image data found in clipboard.");
      return;
    }
  }

  if (screen_selection->allSelected()) {
    all_screen_image = clipboard_image;
    all_screen_filename = CLIPBOARD_IMAGE_MESSAGE;
  } else if (screen_selection->awaySelected()) {
    away_screen_image = clipboard_image;
    away_screen_filename = CLIPBOARD_IMAGE_MESSAGE;
  } else {
    home_screen_image = clipboard_image;
    home_screen_filename = CLIPBOARD_IMAGE_MESSAGE;
  }
  current_file->SetLabelText(CLIPBOARD_IMAGE_MESSAGE);

  control_panel->Update();
  updatePreview();
}

void LocalImage::screenChanged(wxCommandEvent &event) {
  if (screen_selection->allSelected()) {
    if (all_screen_filename.empty()) {
      current_file->SetLabelText(NO_IMAGE_MESSAGE);
    } else {
      current_file->SetLabelText(all_screen_filename);
    }
  } else if (screen_selection->homeSelected()) {
    if (home_screen_filename.empty()) {
      current_file->SetLabelText(NO_IMAGE_MESSAGE);
    } else {
      current_file->SetLabelText(home_screen_filename);
    }
  } else if (screen_selection->awaySelected()) {
    if (away_screen_filename.empty()) {
      current_file->SetLabelText(NO_IMAGE_MESSAGE);
    } else {
      current_file->SetLabelText(away_screen_filename);
    }
  }

  control_panel->Update();
  updatePreview();
}

}  // namespace cszb_scoreboard
