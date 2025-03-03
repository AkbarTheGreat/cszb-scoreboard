/*
ui/Color.h: A wrapper around wxColour which gives convenience methods for
manipulating colors

Copyright 2019-2025 Tracy Beck

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

#include <wx/string.h>  // for wxString

#include <climits>  // for UCHAR_MAX
#include <cstdint>  // for uint64_t

#include "wx/colour.h"  // for wxColour, wxALPHA_OPAQUE

namespace cszb_scoreboard {
class Color : public wxColour {
 public:
  Color() = default;
  Color(unsigned char red, unsigned char green, unsigned char blue,
        unsigned char alpha = wxALPHA_OPAQUE)
      : wxColour(red, green, blue, alpha) {}
  explicit Color(const char *colorName) : wxColour(wxString(colorName)) {}
  explicit Color(const wxString &colorName) : wxColour(colorName) {}
  explicit Color(uint64_t colRGB) : wxColour(colRGB) {}
  explicit Color(const wxColour &color) : wxColour(color) {}

  // Get red by anding by a full byte.
  [[nodiscard]] inline auto red() const -> unsigned char {
    return GetRGB() & UCHAR_MAX;
  }
  // Get green by shifting one byte and anding by a full byte.
  [[nodiscard]] inline auto green() const -> unsigned char {
    // NOLINTNEXTLINE(readability-magic-numbers)
    return (GetRGB() >> 8) & UCHAR_MAX;
  }
  // Get blue by shifting two bytes and anding by a full byte.
  [[nodiscard]] inline auto blue() const -> unsigned char {
    // NOLINTNEXTLINE(readability-magic-numbers)
    return (GetRGB() >> 16) & UCHAR_MAX;
  }
  // Returns a good contrasting color for text over this background
  [[nodiscard]] auto contrastColor() const -> Color;
};

}  // namespace cszb_scoreboard
