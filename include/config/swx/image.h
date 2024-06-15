/*
config/swx/image.h: A wrapper around wxImage with methods removing the need to
use wx types directly.

Copyright 2021-2024 Tracy Beck

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

#pragma once

#include <wx/image.h>   // for wxImage
#include <wx/string.h>  // for wxString

#include <vector>

#include "config/Position.h"      // for Size
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "wx/bitmap.h"            // for wxBitmap

namespace cszb_scoreboard {
class Color;

class Image : public wxImage {
 public:
  // wxImage overridden constructors
  Image() = default;
  explicit Image(const wxImage &img) : wxImage(img) {}

  // Custom constructors
  explicit Image(const ::cszb_scoreboard::Size &sz, bool clear = true)
      : wxImage(sz.toWx(), clear) {}
  explicit Image(const wxBitmap &bmp) : wxImage(bmp.ConvertToImage()) {}
  explicit Image(const FilesystemPath &file) : wxImage(file.string()) {}

  // Generate an Image from data in memory
  static auto FromData(const std::vector<char> &bin_data) -> Image;

  // Custom methods
  auto size() -> ::cszb_scoreboard::Size;
  // Sets the color of the whole image to the given color, but does not change
  // the alpha.
  void setColor(const Color &color);
};

}  // namespace cszb_scoreboard
