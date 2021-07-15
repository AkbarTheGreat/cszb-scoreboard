/*
ui/component/control/ImagePreview.cpp: A preview thumbnail of an image,
selectable to send to a ScreenText.

Copyright 2020-2021 Tracy Beck

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

#include <string>  // for allocator, string

#include "config/Position.h"              // for Size
#include "config/swx/event.h"             // for wxEVT_PAINT
#include "ui/graphics/BackgroundImage.h"  // for BackgroundImage
#include "ui/graphics/Color.h"            // for Color

namespace cszb_scoreboard {

const std::string DEFAULT_PREVIEW_COLOR = "Grey";

ImagePreview::ImagePreview(swx::Panel *wx) : Panel(wx) {
  this->image = BackgroundImage(size(), Color(DEFAULT_PREVIEW_COLOR));
  bindEvents();
}

void ImagePreview::bindEvents() {
  bind(wxEVT_PAINT,
       [this](wxPaintEvent &event) -> void { this->paintEvent(); });
}

void ImagePreview::paintEvent() {
  Image scaled_image = image;
  Size image_size = scaled_image.size();
  float screen_ratio = ratio(size());
  float image_ratio = ratio(image_size);
  int image_height;
  int image_width;
  if (screen_ratio > image_ratio) {
    // Screen is wider than image, so make the heights match
    image_height = size().height;
    image_width = size().height * image_ratio;
  } else {
    // Screen is either the same ratio or narrower than image, so make the
    // widths match
    image_width = size().width;
    image_height = size().width / image_ratio;
  }

  scaled_image.Rescale(image_width, image_height);
  int x = (size().width - image_width) / 2;
  int y = (size().height - image_height) / 2;

  drawImage(scaled_image, x, y);
}

auto ImagePreview::ratio(const Size &size) -> float {
  float ratio = 4 / 3;
  ratio = static_cast<float>(size.width) / size.height;
  return ratio;
}

void ImagePreview::clearImage() {
  image = BackgroundImage(size(), Color(DEFAULT_PREVIEW_COLOR));
  filename.reset();
  refresh();
}

auto ImagePreview::getFilename() const -> std::optional<FilesystemPath> {
  return filename;
}

void ImagePreview::setImage(const FilesystemPath &filename) {
  this->filename = filename;
  image = Image(filename);
  refresh();
}

}  // namespace cszb_scoreboard
