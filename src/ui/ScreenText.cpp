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
#include <wx/tokenzr.h>

#include "ui/BackgroundImage.h"

namespace cszb_scoreboard {

// TODO: Get rid of this event table
BEGIN_EVENT_TABLE(ScreenText, wxPanel)
EVT_PAINT(ScreenText::paintEvent)
END_EVENT_TABLE()

const char* WELCOME_MESSAGE = "Chandler";
const char* ERROR_MESSAGE = "NO SCREENS FOUND!";
const int PREVIEW_HEIGHT = 320;

ScreenText* ScreenText::getPreview(wxWindow* parent, float ratio,
                                   proto::ScreenSide side) {
  wxString initial_text;
  if (side.error()) {
    initial_text = ERROR_MESSAGE;
  } else {
    initial_text = WELCOME_MESSAGE;
  }
  return new ScreenText(parent, initial_text, side,
                        wxSize(PREVIEW_HEIGHT * ratio, PREVIEW_HEIGHT));
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

void ScreenText::setText(const wxString& text, int font_size,
                         const proto::ScreenSide& side) {
  if (isSide(side)) {
    this->text = text;
    this->font_size = font_size;
  }
}

void ScreenText::initializeForColor(wxSize size, Color color) {
  image = BackgroundImage(size, color);
  font_color = color.contrastColor();
}

void ScreenText::renderBackground(wxDC& dc, wxImage image) {
  dc.DrawBitmap(wxBitmap(image, 32), 0, 0, false);
}

void ScreenText::renderText(wxDC& dc, wxString text, Color font_color,
                            wxSize widget_size) {
  wxFont screen_font(wxFontInfo(scaleFont(widget_size))
                         .FaceName("Impact")
                         .Bold()
                         .AntiAliased());
  dc.SetFont(screen_font);
  dc.SetTextForeground(font_color);
  int width, height;
  // dc.GetTextExtent(text, &width, &height);
  getTextExtent(dc, text, &width, &height);
  int x = (widget_size.GetWidth() - width) / 2;
  int y = (widget_size.GetHeight() - height) / 2;
  dc.DrawText(text, x, y);
}

void ScreenText::getTextExtent(wxDC& dc, wxString text, int* width,
                               int* height) {
  wxStringTokenizer tokens(text, "\n\r");
  *width = 0;
  *height = 0;
  wxString token = tokens.GetNextToken();
  while (!token.IsEmpty()) {
    int line_width, line_height;
    dc.GetTextExtent(token, &line_width, &line_height);
    if (line_width > *width) *width = line_width;
    *height += line_height;
    token = tokens.GetNextToken();
  }
}

int ScreenText::scaleFont(wxSize to_size) {
  return to_size.GetHeight() * font_size / 75;
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

void ScreenText::setAll(const ScreenText& source) {
  setImage(source.image);
  if (source.background_color.has_value()) {
    setBackground(*source.background_color);
  }
  setText(source.text, source.font_size, this->screen_side);
  Refresh();
}

bool ScreenText::isSide(proto::ScreenSide side) {
  if (screen_side.home() && side.home()) {
    return true;
  } else if (screen_side.away() && side.away()) {
    return true;
  } else if (screen_side.extra() && side.extra()) {
    return true;
  }
  return false;
}

}  // namespace cszb_scoreboard
