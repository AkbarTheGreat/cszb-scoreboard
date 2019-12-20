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

// TODO: Get rid of this event table
BEGIN_EVENT_TABLE(ScreenText, wxPanel)
EVT_PAINT(ScreenText::paintEvent)
END_EVENT_TABLE()

const char* WELCOME_MESSAGE = "Chandler";
const char* ERROR_MESSAGE = "NO SCREENS FOUND!";

ScreenText* ScreenText::getPreview(wxWindow* parent, proto::ScreenSide side) {
  wxString initial_text;
  if (side.error()) {
    initial_text = ERROR_MESSAGE;
  } else {
    initial_text = WELCOME_MESSAGE;
  }
  return new ScreenText(parent, initial_text, side, wxSize(640, 480));
}

ScreenText* ScreenText::getPresenter(wxWindow* parent, ScreenText* preview,
                                     wxSize size) {
  return new ScreenText(parent, preview->text, preview->image,
                        preview->background_color, preview->font_color,
                        preview->screen_side, size);
}

ScreenText::ScreenText(wxWindow* parent, const wxString& initial_text,
                       wxImage image, std::optional<Color> background_color,
                       Color font_color, proto::ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  text = initial_text;
  this->image = image;
  this->screen_side = side;
  this->font_color = font_color;
  this->background_color = background_color;
  if (background_color.has_value()) {
    initializeForColor(size, *background_color);
  }
}

ScreenText::ScreenText(wxWindow* parent, const wxString& initial_text,
                       proto::ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  this->text = initial_text;
  this->screen_side = side;

  if (side.error()) {
    image = BackgroundImage::errorImage(size);
    font_color = Color(Color("Black"));
  } else {
    background_color = Color("Blue");
    if (side.away()) {
      background_color = Color("Red");
    }
    initializeForColor(size, *background_color);
  }
}

void ScreenText::setText(const wxString& text, const proto::ScreenSide& side) {
  if (screen_side.home() && side.home()) {
    setText(text);
  } else if (screen_side.away() && side.away()) {
    setText(text);
  } else if (screen_side.extra() && side.extra()) {
    setText(text);
  }
}

void ScreenText::initializeForColor(wxSize size, Color color) {
  image = BackgroundImage(size, color);
  font_color = color.contrastColor();
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

void ScreenText::setImage(const wxImage& image) {
  this->background_color.reset();
  this->image = image;
};

void ScreenText::setBackground(const Color color) {
  this->background_color = color;
  initializeForColor(GetSize(), color);
};

}  // namespace cszb_scoreboard
