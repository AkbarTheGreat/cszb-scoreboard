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
#include "ui/component/ScreenText.h"

#include <wx/image.h>
#include <wx/tokenzr.h>

#include "config/TeamConfig.h"
#include "ui/graphics/BackgroundImage.h"

namespace cszb_scoreboard {

ScreenText* ScreenText::getPreview(wxWindow* parent, wxString initial_text,
                                   proto::ScreenSide side, wxSize size) {
  return new ScreenText(parent, initial_text, side, size);
}

ScreenText* ScreenText::getPresenter(wxWindow* parent, ScreenText* preview,
                                     wxSize size) {
  // Note that we copy most elements to the presenter, but specifically not the
  // size.
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
  bindEvents();
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
    std::vector<int> team_indices =
        TeamConfig::getInstance()->indicesForSide(side);
    // TODO: Allow for a view to contain multiple sides
    background_color = TeamConfig::getInstance()->teamColor(team_indices[0]);
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

void ScreenText::bindEvents() {
  Bind(wxEVT_PAINT, &ScreenText::paintEvent, this);
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
  if (!blackout_image.IsOk() ||
      size.GetWidth() != blackout_image.GetSize().GetWidth() ||
      size.GetHeight() != blackout_image.GetSize().GetHeight()) {
    blackout_image = BackgroundImage(size, Color("Black"));
  }
}

void ScreenText::blackout() {
  background_color = Color("Black");
  image = blackout_image;
  text = wxT("");
  Refresh();
}

void ScreenText::renderBackground(wxDC& dc, wxImage image) {
  dc.DrawBitmap(wxBitmap(image, 32), 0, 0, false);
}

void ScreenText::renderText(wxDC& dc, wxString text, Color font_color,
                            wxSize widget_size) {
  wxFont screen_font(
      wxFontInfo(scaleFont(widget_size)).FaceName("Impact").AntiAliased());
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
  } else {
    background_color.reset();
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
