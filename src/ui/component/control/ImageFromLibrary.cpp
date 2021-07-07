/*
ui/component/control/ImageFromLibrary.cpp: Handles loading images from a
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

#include "ui/component/control/ImageFromLibrary.h"

#include "config/ImageLibrary.h"
#include "ui/UiUtil.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const int NUM_PREVIEWS = 5;
const wxSize PREVIEW_SIZE(160, 90);

auto ImageFromLibrary::Create(PreviewPanel *preview_panel, swx::Panel *wx)
    -> std::unique_ptr<ImageFromLibrary> {
  auto library = std::make_unique<ImageFromLibrary>(preview_panel, wx);
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

  for (int i = 0; i < NUM_PREVIEWS; i++) {
    image_previews.push_back(
        new ImagePreview(image_preview_panel->wx, PREVIEW_SIZE));
    image_names.emplace_back(image_preview_panel->label("          "));
  }

  setImages("");

  positionWidgets(control_panel);
  bindEvents();
}

void ImageFromLibrary::positionWidgets(Panel *control_panel) {
  search_panel->addWidget(search_box.get(), 0, 0, DEFAULT_BORDER_SIZE, wxALL);
  search_panel->addWidget(tag_list_label.get(), 1, 0, DEFAULT_BORDER_SIZE,
                          wxALL);

  main_panel->addWidgetWithSpan(search_panel.get(), 0, 0, 1, 2);
  main_panel->addWidget(screen_selection, 0, 2);
  main_panel->addWidget(left_button.get(), 1, 0, DEFAULT_BORDER_SIZE,
                        wxALL | wxALIGN_LEFT);
  main_panel->addWidget(right_button.get(), 1, 1, DEFAULT_BORDER_SIZE,
                        wxALL | wxALIGN_LEFT);

  UiUtil::addToGridBag(control_panel->sizer(), current_image_label, 0, 0);
  control_panel->addWidget(main_panel.get(), 1, 0);
  control_panel->addWidget(image_preview_panel.get(), 2, 0);
  control_panel->addWidget(configure_button.get(), 3, 0, DEFAULT_BORDER_SIZE,
                           wxALL);

  int col = 0;
  for (auto *preview : image_previews) {
    UiUtil::addToGridBag(image_preview_panel->sizer(), preview, 0, col++);
  }

  col = 0;
  for (const auto &name : image_names) {
    image_preview_panel->addWidget(name.get(), 1, col++);
  }

  search_panel->runSizer();
  main_panel->runSizer();
  image_preview_panel->runSizer();
  control_panel->runSizer();
}

void ImageFromLibrary::bindEvents() {
  configure_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->editButton(event); });
  search_box->bind(wxEVT_TEXT, [this](wxCommandEvent &event) -> void {
    this->doSearch(event);
  });
  left_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->pageChange(false); });
  right_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->pageChange(true); });
  for (auto *preview : image_previews) {
    preview->Bind(wxEVT_LEFT_DOWN, &ImageFromLibrary::selectImage, this);
  }
}

void ImageFromLibrary::doSearch(wxCommandEvent &event) {
  setImages(search_box->value());
}

void ImageFromLibrary::editButton(wxCommandEvent &event) {
  edit_dialog = new EditImageLibraryDialog();
  edit_dialog->Create(wx);
  edit_dialog->Show();
  setImages(search_box->value());
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

void ImageFromLibrary::selectImage(wxMouseEvent &event) {
  std::optional<FilesystemPath> filename =
      dynamic_cast<ImagePreview *>(event.GetEventObject())->getFilename();

  // do nothing if someone clicked a gray box
  if (!filename) {
    return;
  }

  if (screen_selection->allSelected()) {
    all_screen_image = wxImage(filename->string());
    all_screen_image_name = ImageLibrary::getInstance()->name(*filename);
    current_image_label->SetLabelText(all_screen_image_name);
  } else if (screen_selection->awaySelected()) {
    away_screen_image = wxImage(filename->string());
    away_screen_image_name = ImageLibrary::getInstance()->name(*filename);
    current_image_label->SetLabelText(away_screen_image_name);
  } else {
    home_screen_image = wxImage(filename->string());
    home_screen_image_name = ImageLibrary::getInstance()->name(*filename);
    current_image_label->SetLabelText(home_screen_image_name);
  }

  control_panel->update();
  updatePreview();
}

void ImageFromLibrary::setImages(const wxString &search,
                                 unsigned int page_number) {
  current_image_page = page_number;

  ImageSearchResults results =
      ImageLibrary::getInstance()->search(std::string(search));

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
    image_names[i - start_num]->set(
        ImageLibrary::getInstance()->name(files[i]));
  }

  for (int i = stop_num - start_num; i < NUM_PREVIEWS; i++) {
    image_previews[i]->clearImage();
    image_names[i]->set("");
  }
}

}  // namespace cszb_scoreboard
