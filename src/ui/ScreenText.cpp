/*
ui/ScreenText.cpp: Represents a text presentation on a ScreenProvider.

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
#include "ui/ScreenText.h"
#include <wx/image.h>
#include "ui/BackgroundImage.h"

namespace cszb_scoreboard {

BEGIN_EVENT_TABLE(ScreenText, wxPanel)
EVT_PAINT(ScreenText::paintEvent)
END_EVENT_TABLE()

Color ContrastColor(Color fontColor);

ScreenText* ScreenText::getPreview(wxWindow* parent,
                                   const wxString& initial_text,
                                   ScreenSide side) {
  return new ScreenText(parent, initial_text, side, wxSize(640, 480));
}

ScreenText* ScreenText::getPresenter(wxWindow* parent, ScreenText* preview,
                                     wxSize size) {
  return new ScreenText(parent, preview->text, preview->side, size);
}

ScreenText::ScreenText(wxWindow* parent, const wxString& initial_text,
                       ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  this->text = initial_text;
  this->side = side;

  switch (side) {
    case SIDE_SINGLE:  // TODO: Build single screen functionality here
    case SIDE_LEFT:
      image = BackgroundImage(size, Color("Blue"));
      font_color = ContrastColor(Color("Blue"));
      break;
    case SIDE_RIGHT:
      image = BackgroundImage(size, Color("Red"));
      font_color = ContrastColor(Color("Red"));
      break;
    case SIDE_NONE:
    default:
      image = BackgroundImage::errorImage(size);
      font_color = Color(Color("Black"));
      break;
  }
}

void renderBackground(wxDC& dc, wxImage image) {
  dc.DrawBitmap(wxBitmap(image, 32), 0, 0, false);
}

void renderText(wxDC& dc, wxString text, Color font_color, wxSize widget_size) {
  wxFont screen_font(wxFontInfo(64).FaceName("Impact").Bold().AntiAliased());
  dc.SetFont(screen_font);
  dc.SetTextForeground(font_color);
  int width, height;
  dc.GetTextExtent(text, &width, &height);
  int x = (widget_size.GetWidth() - width) / 2;
  int y = (widget_size.GetHeight() - height) / 2;
  dc.DrawText(text, x, y);
}

void ScreenText::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  renderBackground(dc, image);
  renderText(dc, text, font_color, this->GetSize());
}

// Taken from
// https://stackoverflow.com/questions/1855884/determine-font-color-based-on-background-color
// as a way to calculate font color to contrast effectively against an arbitrary
// background.
Color ContrastColor(Color backgroundColor) {
  int d = 0;

  int rgb = backgroundColor.GetRGB();
  int blue = (rgb >> 16) & 0xFF;
  int green = (rgb >> 8) & 0xFF;
  int red = rgb & 0xFF;
  // Counting the perceptive luminance - human eye favors green color...
  double luminance = (0.299 * red + 0.587 * green + 0.114 * blue) / 255;

  if (luminance > 0.5)
    d = 0;  // bright colors - black font
  else
    d = 255;  // dark colors - white font

  return Color(d, d, d);
}
}  // namespace cszb_scoreboard
