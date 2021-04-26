/*
ui/component/control/LocalImage.cpp: Handles loading images from disk or from
the clipboard.

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

#include "ui/component/control/LocalImage.h"

#include <wx/clipbrd.h>

#include "ui/UiUtil.h"
#include "util/FilesystemPath.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const std::string CLIPBOARD_IMAGE_MESSAGE = "<Image Loaded From Clipboard>";

auto LocalImage::Create(PreviewPanel *preview_panel, wxWindow *parent)
    -> LocalImage * {
  auto *local_image = new LocalImage(preview_panel, parent);
  local_image->initializeWidgets();
  local_image->updatePreview();
  return local_image;
}

void LocalImage::createControls(wxPanel *control_panel) {
  ScreenImageController::createControls(control_panel);
  inner_panel = new wxPanel(control_panel);

  // Reparent our screen_selection to position it into inner_panel, for layout.
  screen_selection->Reparent(inner_panel);

  button_panel = new wxPanel(inner_panel);
  browse_button = new wxButton(button_panel, wxID_ANY, "Browse");
  paste_button = new wxButton(button_panel, wxID_ANY, "Load From Clipboard");
  positionWidgets(control_panel);
  bindEvents();
}

void LocalImage::positionWidgets(wxPanel *control_panel) {
  wxSizer *outer_sizer = UiUtil::sizer(0, 1);
  wxSizer *inner_sizer = UiUtil::sizer(0, 2);
  wxSizer *button_sizer = UiUtil::sizer(2, 0);

  button_sizer->Add(browse_button, 0, wxALL, BORDER_SIZE);
  button_sizer->Add(paste_button, 0, wxALL, BORDER_SIZE);

  button_panel->SetSizerAndFit(button_sizer);

  inner_sizer->Add(button_panel, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);

  inner_panel->SetSizerAndFit(inner_sizer);

  outer_sizer->Add(current_image_label, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(inner_panel, 0, wxALL, 0);

  control_panel->SetSizerAndFit(outer_sizer);
}

void LocalImage::bindEvents() {
  browse_button->Bind(wxEVT_BUTTON, &LocalImage::browsePressed, this);
  paste_button->Bind(wxEVT_BUTTON, &LocalImage::pastePressed, this);
}

void LocalImage::browsePressed(wxCommandEvent &event) {
  wxFileDialog dialog(this, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() != wxID_CANCEL) {
    FilesystemPath selected_file =
        FilesystemPath(std::string(dialog.GetPath()));
    if (screen_selection->allSelected()) {
      all_screen_image = wxImage(selected_file.c_str());
      all_screen_image_name = selected_file.filename().string();
    } else if (screen_selection->awaySelected()) {
      away_screen_image = wxImage(selected_file.c_str());
      away_screen_image_name = selected_file.filename().string();
    } else {
      home_screen_image = wxImage(selected_file.c_str());
      home_screen_image_name = selected_file.filename().string();
    }
    current_image_label->SetLabelText(selected_file.filename().c_str());
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
    all_screen_image_name = CLIPBOARD_IMAGE_MESSAGE;
  } else if (screen_selection->awaySelected()) {
    away_screen_image = clipboard_image;
    away_screen_image_name = CLIPBOARD_IMAGE_MESSAGE;
  } else {
    home_screen_image = clipboard_image;
    home_screen_image_name = CLIPBOARD_IMAGE_MESSAGE;
  }
  current_image_label->SetLabelText(CLIPBOARD_IMAGE_MESSAGE);

  control_panel->Update();
  updatePreview();
}

}  // namespace cszb_scoreboard
