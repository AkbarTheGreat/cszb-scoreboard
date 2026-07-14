/*
ui/widget/Image.h: A widget wrapper around swx::Image with custom animation and
layout helpers.

Copyright 2021-2026 Tracy Beck

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

#include <stdint.h>     // for int64_t
#include <wx/bitmap.h>  // for wxBitmap

#include <memory>  // for shared_ptr
#include <vector>  // for vector

#include "config/Position.h"   // for Size
#include "config/swx/image.h"  // for Image
#include "wx/gdicmn.h"         // for wxRect
#include "wx/image.h"          // for wxImageResizeQuality, wxImage

namespace cszb_scoreboard {
class Color;
class FilesystemPath;

class Image {
 public:
  // Constructors
  Image() = default;
  // NOLINTNEXTLINE(modernize-pass-by-value)
  explicit Image(const swx::Image& img) : _wx(img) {}
  // NOLINTNEXTLINE(modernize-pass-by-value)
  explicit Image(const wxImage& img) : _wx(img) {}
  explicit Image(const ::cszb_scoreboard::Size& sz, bool clear = true);
  explicit Image(const wxBitmap& bmp) : _wx(bmp) {}
  explicit Image(const FilesystemPath& file);
  explicit Image(const std::vector<char>& bin_data);

  // Custom methods
  [[nodiscard]] auto size() const -> ::cszb_scoreboard::Size;
  void color(const Color& color);

  // Animation methods
  [[nodiscard]] auto isAnimated() const -> bool;
  [[nodiscard]] auto animate(int64_t current_time_ms) const -> wxImage;
  auto rescale(int width, int height,
               wxImageResizeQuality quality = wxIMAGE_QUALITY_NORMAL) -> Image&;

  // Wrapped swx::Image methods (cased to standard style)
  [[nodiscard]] auto isOk() const -> bool;
  [[nodiscard]] auto alpha() const -> unsigned char*;
  [[nodiscard]] auto red(int x, int y) const -> unsigned char;
  [[nodiscard]] auto green(int x, int y) const -> unsigned char;
  [[nodiscard]] auto blue(int x, int y) const -> unsigned char;
  void initAlpha();
  void alpha(unsigned char* alpha);
  void rgb(const wxRect& rect, unsigned char r, unsigned char g,
           unsigned char b);
  void rgb(int x, int y, unsigned char r, unsigned char g, unsigned char b);

  // Named wxImage accessors
  [[nodiscard]] auto wx() const -> const wxImage& { return _wx; }
  [[nodiscard]] auto wx() -> wxImage& { return _wx; }

 private:
  swx::Image _wx;
  std::shared_ptr<std::vector<wxImage>> frames;
  std::shared_ptr<std::vector<int>> delays;
  int64_t start_time = 0;

  void loadAnimation(const FilesystemPath& file);
};

}  // namespace cszb_scoreboard
