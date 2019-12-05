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

namespace cszb_scoreboard {

BEGIN_EVENT_TABLE(ScreenText, wxPanel)
EVT_PAINT(ScreenText::paintEvent)
END_EVENT_TABLE()

const wxString ERROR_IMAGE(
    "C:\\Users\\akbar\\Documents\\Code\\Vs_ws\\cszb-"
    "scoreboard\\resources\\error.png");

wxColour ContrastColor(wxColour fontColor);

void ScreenText::setColors(wxColour backgroundColor) {
  int rgb = backgroundColor.GetRGB();
  int blue = (rgb >> 16) & 0xFF;
  int green = (rgb >> 8) & 0xFF;
  int red = rgb & 0xFF;
  wxRect fullMask(0, 0, image.GetWidth(), image.GetHeight());
  image.SetRGB(fullMask, red, green, blue);
  fontColor = ContrastColor(backgroundColor);
}

ScreenText* ScreenText::GetPreview(wxWindow* parent,
                                   const wxString& initialText,
                                   ScreenSide side) {
  return new ScreenText(parent, initialText, side, wxSize(640, 480));
}

ScreenText* ScreenText::GetPresenter(wxWindow* parent, ScreenText *preview, wxSize size) {
  return new ScreenText(parent, preview->text, preview->side, size);
}

ScreenText::ScreenText(wxWindow* parent, const wxString& initialText,
                       ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  this->text = initialText;
  this->side = side;

  image.LoadFile(ERROR_IMAGE, wxBITMAP_TYPE_PNG);
  switch (side) {
    case SIDE_LEFT:
      setColors(wxColour("Blue"));
      break;
    case SIDE_RIGHT:
      setColors(wxColour("Red"));
      break;
    case SIDE_NONE:
    default:
      image.LoadFile(ERROR_IMAGE, wxBITMAP_TYPE_PNG);
      fontColor = wxColour("Black");
      break;
  }
}

void renderBackground(wxDC& dc, wxImage image) {
  wxBitmap bmp(image, 32);
  dc.DrawBitmap(bmp, 0, 0, false);
}

void renderText(wxDC& dc, wxString text, wxColour fontColor, wxSize widgetSize) {
  wxFont screenFont(wxFontInfo(64).FaceName("Impact").Bold().AntiAliased());
  dc.SetFont(screenFont);
  dc.SetTextForeground(fontColor);
  int width, height;
  dc.GetTextExtent(text, &width, &height);
  int x = (widgetSize.GetWidth() - width) / 2;
  int y = (widgetSize.GetHeight() - height) / 2;
  dc.DrawText(text, x, y);
}

void ScreenText::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  renderBackground(dc, image);
  renderText(dc, text, fontColor, this->GetSize());
}

// Taken from
// https://stackoverflow.com/questions/1855884/determine-font-color-based-on-background-color
// as a way to calculate font color to contrast effectively against an arbitrary
// background.
wxColour ContrastColor(wxColour backgroundColor) {
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

  return wxColour(d, d, d);
}
}  // namespace cszb_scoreboard
