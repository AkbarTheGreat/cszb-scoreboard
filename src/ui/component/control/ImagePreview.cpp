/*
ui/component/control/ImagePreview.cpp: A preview thumbnail of an image,
selectable to send to a ScreenText.

Copyright 2020 Tracy Beck

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

#include "ui/component/control/ImagePreview.h"

#include "ui/graphics/BackgroundImage.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

ImagePreview::ImagePreview(wxWindow* parent, const wxSize& size,
                           const wxImage& image)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  this->image = image;
  this->size = size;
  bindEvents();
}  // namespace cszb_scoreboard

ImagePreview::ImagePreview(wxWindow* parent, const wxSize& size,
                           std::string image_filename)
    : ImagePreview(parent, size, wxImage(image_filename)) {}

ImagePreview::ImagePreview(wxWindow* parent, const wxSize& size)
    : ImagePreview(parent, size, BackgroundImage(size, Color("Grey"))) {}

void ImagePreview::bindEvents() {
  Bind(wxEVT_PAINT, &ImagePreview::paintEvent, this);
}

void ImagePreview::renderImage(wxDC& dc) {
  wxImage scaled_image = image;
  wxSize image_size = scaled_image.GetSize();
  float screen_ratio = ratio(size);
  float image_ratio = ratio(image_size);
  int image_height, image_width;
  if (screen_ratio > image_ratio) {
    // Screen is wider than image, so make the heights match
    image_height = size.GetHeight();
    image_width = size.GetHeight() * image_ratio;
  } else {
    // Screen is either the same ratio or narrower than image, so make the
    // widths match
    image_width = size.GetWidth();
    image_height = size.GetWidth() / image_ratio;
  }

  scaled_image.Rescale(image_width, image_height);
  int x = (size.GetWidth() - image_width) / 2;
  int y = (size.GetHeight() - image_height) / 2;

  dc.DrawBitmap(wxBitmap(scaled_image, 32), x, y, false);
}

void ImagePreview::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  renderImage(dc);
}

float ImagePreview::ratio(const wxSize& size) {
  float ratio = 4 / 3;
  ratio = (float)size.GetWidth() / size.GetHeight();
  return ratio;
}

}  // namespace cszb_scoreboard
