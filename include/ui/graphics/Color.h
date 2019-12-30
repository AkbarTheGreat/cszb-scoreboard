/*
ui/Color.h: A wrapper around wxColour which gives convenience methods for
manipulating colors

Copyright 2019 Tracy Beck

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

namespace cszb_scoreboard {
class Color : public wxColour {
 public:
  Color() : wxColour() {}
  Color(unsigned char red, unsigned char green, unsigned char blue,
        unsigned char alpha = wxALPHA_OPAQUE)
      : wxColour(red, green, blue, alpha) {}
  Color(const char *colorName) : wxColour(wxString(colorName)) {}
  Color(const wxString &colorName) : wxColour(colorName) {}
  Color(unsigned long colRGB) : wxColour(colRGB) {}
  Color(const wxColour &color) : wxColour(color) {}

  inline unsigned char red() { return GetRGB() & 0xFF; }
  inline unsigned char green() { return (GetRGB() >> 8) & 0xFF; }
  inline unsigned char blue() { return (GetRGB() >> 16) & 0xFF; }
  // Returns a good contrasting color for text over this background
  Color contrastColor();
};

}  // namespace cszb_scoreboard
