/*
config/swx/image.h: A wrapper around wxImage with methods removing the need to
use wx types directly.

Copyright 2021 Tracy Beck

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

#include <wx/wx.h>

#include "config/Position.h"
#include "wx/bitmap.h"  // for wxBitmap

namespace cszb_scoreboard {

class Image : public wxImage {
 public:
  Image() = default;
  explicit Image(const ::cszb_scoreboard::Size &sz, bool clear = true)
      : wxImage(sz.toWx(), clear) {}
  explicit Image(const wxSize &sz, bool clear = true) : wxImage(sz, clear) {}
  explicit Image(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_ANY,
                 int index = -1)
      : wxImage(name, type, index) {}
  Image(const wxBitmap &bmp) : wxImage(bmp.ConvertToImage()) {}
  Image(const wxImage &img);
  auto size() -> ::cszb_scoreboard::Size;
};

}  // namespace cszb_scoreboard
