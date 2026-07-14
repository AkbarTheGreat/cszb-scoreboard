/*
config/swx/image.h: A wrapper around wxImage with methods removing the need to
use wx types directly.

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

#include <wx/bitmap.h>  // for wxBitmap
#include <wx/image.h>   // for wxImage
#include <wx/string.h>  // for wxString

namespace cszb_scoreboard::swx {

class Image : public wxImage {
 public:
  Image() = default;
  explicit Image(const wxImage& img) : wxImage(img) {}
  explicit Image(const wxSize& sz, bool clear = true) : wxImage(sz, clear) {}
  explicit Image(const wxBitmap& bmp) : wxImage(bmp.ConvertToImage()) {}
  explicit Image(const wxString& file) : wxImage(file) {}
};

}  // namespace cszb_scoreboard::swx
