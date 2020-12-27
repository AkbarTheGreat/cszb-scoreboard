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

#include "config/ImageLibrary.h"
#include "ui/UiUtil.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const int NUM_PREVIEWS = 5;
const wxSize PREVIEW_SIZE(160, 90);

auto ImageFromLibrary::Create(PreviewPanel *preview_panel, wxWindow *parent)
    -> ImageFromLibrary * {
  auto *library = new ImageFromLibrary(preview_panel, parent);
  library->initializeWidgets();
  library->updatePreview();
  return library;
}

void ImageFromLibrary::createControls(wxPanel *control_panel) {
  ScreenImageController::createControls(control_panel);
  main_panel = new wxPanel(control_panel);
  image_preview_panel = new wxPanel(control_panel);
  search_panel = new wxPanel(main_panel);

  // Reparent our screen_selection to position it into inner_panel, for layout.
  screen_selection->Reparent(main_panel);

  left_button = new wxButton(main_panel, wxID_ANY, "<");
  right_button = new wxButton(main_panel, wxID_ANY, ">");
  configure_button = new wxButton(control_panel, wxID_ANY, "Edit Library");

  search_box = new wxSearchCtrl(search_panel, wxID_ANY);
  search_box->SetDescriptiveText("Find by tag/name");
  search_box->ShowSearchButton(false);
  search_box->ShowCancelButton(true);
  tag_list_label = new wxStaticText(search_panel, wxID_ANY, "");

  for (int i = 0; i < NUM_PREVIEWS; i++) {
    image_previews.push_back(
        new ImagePreview(image_preview_panel, PREVIEW_SIZE));
    image_names.push_back(
        new wxStaticText(image_preview_panel, wxID_ANY, "          "));
  }

  setImages("");

  positionWidgets(control_panel);
  bindEvents();
}

void ImageFromLibrary::positionWidgets(wxPanel *control_panel) {
  wxSizer *main_sizer = UiUtil::sizer(0, 1);
  wxSizer *main_panel_sizer = UiUtil::sizer(0, 2);
  wxSizer *search_panel_sizer = UiUtil::sizer(0, 1);
  wxSizer *image_preview_sizer = UiUtil::sizer(0, NUM_PREVIEWS);

  search_panel_sizer->Add(search_box, 0, wxALL, BORDER_SIZE);
  search_panel_sizer->Add(tag_list_label, 0, wxALL, BORDER_SIZE);

  main_panel_sizer->Add(search_panel, 0, wxALL, BORDER_SIZE);
  main_panel_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);
  main_panel_sizer->Add(left_button, 0, wxALL, BORDER_SIZE);
  main_panel_sizer->Add(right_button, 0, wxALL, BORDER_SIZE);

  main_sizer->Add(current_image_label, 0, wxALL, BORDER_SIZE);
  main_sizer->Add(main_panel, 0, wxALL, BORDER_SIZE);
  main_sizer->Add(image_preview_panel, 0, wxALL, BORDER_SIZE);
  main_sizer->Add(configure_button, 0, wxALL, BORDER_SIZE);

  for (auto *preview : image_previews) {
    image_preview_sizer->Add(preview, 0, wxALL, BORDER_SIZE);
  }

  for (auto *name : image_names) {
    image_preview_sizer->Add(name, 0, wxALL, BORDER_SIZE);
  }

  search_panel->SetSizerAndFit(search_panel_sizer);
  main_panel->SetSizerAndFit(main_panel_sizer);
  image_preview_panel->SetSizerAndFit(image_preview_sizer);
  control_panel->SetSizerAndFit(main_sizer);
}

void ImageFromLibrary::bindEvents() {
  configure_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                         &ImageFromLibrary::editButton, this);
  search_box->Bind(wxEVT_TEXT, &ImageFromLibrary::doSearch, this);
  left_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageFromLibrary::pageChange,
                    this);
  right_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                     &ImageFromLibrary::pageChange, this);
  for (auto *preview : image_previews) {
    preview->Bind(wxEVT_LEFT_DOWN, &ImageFromLibrary::selectImage, this);
  }
}

void ImageFromLibrary::doSearch(wxCommandEvent &event) {
  setImages(search_box->GetValue());
}

void ImageFromLibrary::editButton(wxCommandEvent &event) {
  edit_dialog = new EditImageLibraryDialog();
  edit_dialog->Create(this);
  edit_dialog->Show();
  setImages(search_box->GetValue());
}

void ImageFromLibrary::pageChange(wxCommandEvent &event) {
  if (event.GetEventObject() == left_button) {
    if (current_image_page > 0) {
      setImages(search_box->GetValue(), current_image_page - 1);
    }
  } else {
    setImages(search_box->GetValue(), current_image_page + 1);
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

  control_panel->Update();
  updatePreview();
}

void ImageFromLibrary::setImages(const wxString &search,
                                 unsigned int page_number) {
  current_image_page = page_number;

  ImageSearchResults results =
      ImageLibrary::getInstance()->search(std::string(search));

  if (search.empty()) {
    tag_list_label->SetLabel("");
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
    tag_list_label->SetLabel(tag_string);
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
    image_names[i - start_num]->SetLabelText(
        ImageLibrary::getInstance()->name(files[i]));
  }

  for (int i = stop_num - start_num; i < NUM_PREVIEWS; i++) {
    image_previews[i]->clearImage();
    image_names[i]->SetLabelText("");
  }
}

}  // namespace cszb_scoreboard
