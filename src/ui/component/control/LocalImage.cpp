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

#include <wx/defs.h>         // for wxDF_BITMAP, wxID_CANCEL
#include <wx/event.h>        // for wxCommandEvent (ptr o...
#include <wx/image.h>        // for wxImage
#include <wx/msgdlg.h>       // for wxMessageBox
#include <wx/string.h>       // for wxString
#include <wx/translation.h>  // for _

#include <filesystem>  // for path
#include <string>      // for string

#include "ScoreboardCommon.h"                   // for DEFAULT_BORDER_SIZE
#include "ui/component/control/TeamSelector.h"  // for TeamSelector
#include "ui/widget/Label.h"                    // for Label
#include "ui/widget/Widget.h"                   // for NO_BORDER
#include "ui/widget/swx/Panel.h"                // for Panel
#include "util/Clipboard.h"
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/Log.h"             // IWYU pragma: keep for LogDebug
#include "wx/bitmap.h"            // for wxBitmap
#include "wx/filedlg.h"           // for wxFileDialog, wxFD_FI...
// IWYU pragma: no_include <wx/gtk/clipbrd.h>
// IWYU pragma: no_include <wx/gtk/dataobj2.h>

namespace cszb_scoreboard {

class PreviewPanel;

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
  button_panel->addWidget(*browse_button, 0, 0);
  button_panel->addWidget(*paste_button, 1, 0);

  inner_panel->addWidget(*button_panel, 0, 0);
  inner_panel->addWidget(*screen_selection, 0, 1);

  control_panel->addWidget(*current_image_label, 0, 0);
  control_panel->addWidget(*inner_panel, 1, 0, NO_BORDER);

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
  std::optional<Image> clipboard_image = Clipboard::getImage();

  if (!clipboard_image.has_value()) {
    wxMessageBox("ERROR: No supported image data found in clipboard.");
    return;
  }

  if (screen_selection->allSelected()) {
    all_screen_image = *clipboard_image;
    all_screen_image_name = CLIPBOARD_IMAGE_MESSAGE;
  } else if (screen_selection->awaySelected()) {
    away_screen_image = *clipboard_image;
    away_screen_image_name = CLIPBOARD_IMAGE_MESSAGE;
  } else {
    home_screen_image = *clipboard_image;
    home_screen_image_name = CLIPBOARD_IMAGE_MESSAGE;
  }
  current_image_label->set(CLIPBOARD_IMAGE_MESSAGE);

  control_panel->update();
  updatePreview();
}

}  // namespace cszb_scoreboard
