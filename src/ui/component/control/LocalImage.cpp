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

auto LocalImage::Create(PreviewPanel *preview_panel, swx::Panel *wx)
    -> std::unique_ptr<LocalImage> {
  auto local_image = std::make_unique<LocalImage>(preview_panel, wx);
  local_image->initializeWidgets();
  local_image->updatePreview();
  return local_image;
}

void LocalImage::createControls(Panel *control_panel) {
  ScreenImageController::createControls(control_panel);
  inner_panel = control_panel->panel();

  // Reparent our screen_selection to position it into inner_panel, for layout.
  screen_selection->setParent(inner_panel.get());

  button_panel = inner_panel->panel();
  browse_button = button_panel->button("Browse");
  paste_button = button_panel->button("Load From Clipboard");
  positionWidgets(control_panel);
  bindEvents();
}

void LocalImage::positionWidgets(Panel *control_panel) {
  // wxSizer *inner_sizer = UiUtil::sizer(0, 2);
  // wxSizer *button_sizer = UiUtil::sizer(2, 0);

  button_panel->addWidget(browse_button.get(), 0, 0);
  button_panel->addWidget(paste_button.get(), 1, 0);

  inner_panel->addWidget(button_panel.get(), 0, 0);
  inner_panel->addWidget(screen_selection.get(), 0, 1);

  // wxSizer *outer_sizer = UiUtil::sizer(0, 1);
  control_panel->addWidget(current_image_label.get(), 0, 0);
  control_panel->addWidget(inner_panel.get(), 1, 0, NO_BORDER);

  button_panel->runSizer();
  inner_panel->runSizer();
  control_panel->runSizer();
}

void LocalImage::bindEvents() {
  browse_button->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->browsePressed();
  });
  paste_button->bind(wxEVT_BUTTON, [this](wxCommandEvent &event) -> void {
    this->pastePressed();
  });
}

void LocalImage::browsePressed() {
  wxFileDialog dialog(wx, _("Select Image"), "", "", IMAGE_SELECTION_STRING,
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
    current_image_label->set(selected_file.filename().string());
  }

  control_panel->update();
  updatePreview();
}

void LocalImage::pastePressed() {
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
  current_image_label->set(CLIPBOARD_IMAGE_MESSAGE);

  control_panel->update();
  updatePreview();
}

}  // namespace cszb_scoreboard
