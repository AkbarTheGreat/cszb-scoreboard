/*
ui/component/control/ImageFromLibrary.cpp: Handles loading images from a
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

#include "ui/component/control/ImageFromLibrary.h"

#include <optional>  // for optional
#include <string>    // for string

#include "ScoreboardCommon.h"                   // for DEFAULT_BORDER_SIZE
#include "config/ImageLibrary.h"                // for ImageLibrary, ImageSe...
#include "config/swx/defs.h"                    // for wxALL, wxALIGN_LEFT
#include "config/swx/event.h"                   // for wxEVT_COMMAND_BUTTON_...
#include "config/swx/image.h"                   // for Image
#include "ui/component/control/TeamSelector.h"  // for TeamSelector
#include "ui/dialog/EditImageLibraryDialog.h"   // for EditImageLibraryDialog
#include "util/FilesystemPath.h"                // for FilesystemPath
#include "util/Singleton.h"                     // for Singleton

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const int NUM_PREVIEWS = 5;
const int PREVIEW_WIDTH = 160;
const int PREVIEW_HEIGHT = 90;

auto ImageFromLibrary::Create(swx::Panel *wx)
    -> std::unique_ptr<ImageFromLibrary> {
  auto library = std::make_unique<ImageFromLibrary>(wx);
  library->initializeWidgets();
  library->updatePreview();
  return library;
}

void ImageFromLibrary::createControls(Panel *control_panel) {
  ScreenImageController::createControls(control_panel);
  main_panel = control_panel->panel();
  image_preview_panel = control_panel->panel();
  search_panel = main_panel->panel();

  // Reparent our screen_selection to position it into inner_panel, for layout.
  screen_selection->setParent(main_panel.get());

  left_button = main_panel->button("<");
  right_button = main_panel->button(">");
  configure_button = control_panel->button("Edit Library");

  search_box = search_panel->searchBox("Find by tag/name");
  tag_list_label = search_panel->label("");

  image_previews.reserve(NUM_PREVIEWS);
  image_names.reserve(NUM_PREVIEWS);
  for (int i = 0; i < NUM_PREVIEWS; i++) {
    image_previews.emplace_back(std::make_unique<ImagePreview>(
        image_preview_panel->childPanel(PREVIEW_WIDTH, PREVIEW_HEIGHT)));
    image_names.emplace_back(image_preview_panel->label("          "));
  }

  setImages("");

  positionWidgets(control_panel);
  bindEvents();
}

void ImageFromLibrary::positionWidgets(Panel *control_panel) {
  search_panel->addWidget(*search_box, 0, 0, DEFAULT_BORDER_SIZE, wxALL);
  search_panel->addWidget(*tag_list_label, 1, 0, DEFAULT_BORDER_SIZE, wxALL);

  main_panel->addWidgetWithSpan(*search_panel, 0, 0, 1, 2);
  main_panel->addWidget(*screen_selection, 0, 2);
  main_panel->addWidget(*left_button, 1, 0, DEFAULT_BORDER_SIZE,
                        wxALL | wxALIGN_LEFT);
  main_panel->addWidget(*right_button, 1, 1, DEFAULT_BORDER_SIZE,
                        wxALL | wxALIGN_LEFT);

  control_panel->addWidget(*current_image_label, 0, 0);
  control_panel->addWidget(*main_panel, 1, 0);
  control_panel->addWidget(*image_preview_panel, 2, 0);
  control_panel->addWidget(*configure_button, 3, 0, DEFAULT_BORDER_SIZE, wxALL);

  int col = 0;
  for (const auto &preview : image_previews) {
    image_preview_panel->addWidget(*preview, 0, col++);
  }

  col = 0;
  for (const auto &name : image_names) {
    image_preview_panel->addWidget(*name, 1, col++);
  }

  search_panel->runSizer();
  main_panel->runSizer();
  image_preview_panel->runSizer();
  control_panel->runSizer();
}

void ImageFromLibrary::bindEvents() {
  configure_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->editButton(); });
  search_box->bind(wxEVT_TEXT,
                   [this](wxCommandEvent &event) -> void { this->doSearch(); });
  left_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->pageChange(false); });
  right_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->pageChange(true); });
  for (const auto &preview : image_previews) {
    // Capture the unique_ptr to a referencing pointer, for the lambda capture.
    auto *arg = preview.get();
    preview->bind(wxEVT_LEFT_DOWN, [this, arg](wxMouseEvent &event) -> void {
      this->selectImage(arg);
    });
  }
}

void ImageFromLibrary::doSearch() { setImages(search_box->value()); }

void ImageFromLibrary::editButton() {
  edit_dialog = std::make_unique<EditImageLibraryDialog>(
      childDialog("Edit Image Library"), this);
  edit_dialog->show();
}

void ImageFromLibrary::onEditDialogClose() {
  edit_dialog.reset();
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog. Also update the images shown now that
  // it's exited.
  setImages(search_box->value());
  focus();
}

void ImageFromLibrary::pageChange(bool forward) {
  if (forward) {
    setImages(search_box->value(), current_image_page + 1);
  } else {
    if (current_image_page > 0) {
      setImages(search_box->value(), current_image_page - 1);
    }
  }
}

// selectImage uses a pointer instead of a const ref due to the delayed nature
// of it's use in a bind.  If you bind the lambda with a const ref, you wind up
// with an object that is potentially stuck in initialization-time semantics.
void ImageFromLibrary::selectImage(ImagePreview *image) {
  std::optional<FilesystemPath> filename = image->getFilename();

  // do nothing if someone clicked a gray box
  if (!filename) {
    return;
  }

  if (screen_selection->allSelected()) {
    all_screen_image = Image(*filename);
    all_screen_image_name = singleton->imageLibrary()->name(*filename);
    current_image_label->set(all_screen_image_name);
  } else if (screen_selection->awaySelected()) {
    away_screen_image = Image(*filename);
    away_screen_image_name = singleton->imageLibrary()->name(*filename);
    current_image_label->set(away_screen_image_name);
  } else {
    home_screen_image = Image(*filename);
    home_screen_image_name = singleton->imageLibrary()->name(*filename);
    current_image_label->set(home_screen_image_name);
  }

  control_panel->update();
  updatePreview();
}

void ImageFromLibrary::setImages(const std::string &search,
                                 unsigned int page_number) {
  current_image_page = page_number;

  ImageSearchResults results =
      singleton->imageLibrary()->search(std::string(search));

  if (search.empty()) {
    tag_list_label->set("");
  } else {
    std::string tag_string;
    bool first = true;
    for (const auto &tag : results.matchedTags()) {
      if (!first) {
        tag_string += ", ";
      }
      tag_string += tag;
      first = false;
    }
    tag_list_label->set(tag_string);
  }

  std::vector<FilesystemPath> files = results.filenames();
  if (files.empty()) {
    current_image_page = 0;
    return;
  }

  int start_num = NUM_PREVIEWS * page_number;

  if (start_num >= files.size()) {
    return setImages(search, page_number - 1);
  }

  int stop_num = start_num + NUM_PREVIEWS;
  if (stop_num >= files.size()) {
    stop_num = files.size();
  }

  for (int i = start_num; i < stop_num; i++) {
    image_previews[i - start_num]->setImage(files[i]);
    image_names[i - start_num]->set(singleton->imageLibrary()->name(files[i]));
  }

  for (int i = stop_num - start_num; i < NUM_PREVIEWS; i++) {
    image_previews[i]->clearImage();
    image_names[i]->set("");
  }
}

}  // namespace cszb_scoreboard
