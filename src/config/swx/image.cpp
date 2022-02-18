/*
config/swx/image.cpp: A wrapper around wxImage with methods removing the need to
use wx types directly.

Copyright 2021-2022 Tracy Beck

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

#include "config/swx/image.h"

#include <wx/gdicmn.h>  // for wxPoint, wxRect
#include <wx/mstream.h>

#include "ui/graphics/Color.h"  // for Color

namespace cszb_scoreboard {

auto Image::size() -> ::cszb_scoreboard::Size {
  return ::cszb_scoreboard::Size::fromWx(GetSize());
}

void Image::setColor(const Color &color) {
  wxRect dimensions(wxPoint(0, 0), GetSize());
  SetRGB(dimensions, color.Red(), color.Green(), color.Blue());
}

auto Image::FromData(const std::vector<char> &bin_data) -> Image {
  wxMemoryInputStream inputStream(bin_data.data(), bin_data.size());
  wxImage img(inputStream, wxBITMAP_TYPE_ANY, -1);
  return Image(img);
}

}  // namespace cszb_scoreboard
