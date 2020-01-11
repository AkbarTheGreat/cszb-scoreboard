/*
ui/component/ScreenText.cpp: Represents a text presentation on a ScreenPresenter
or a ScreenPreview.

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
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

ScreenText* ScreenText::getPreview(wxWindow* parent, wxString initial_text,
                                   proto::ScreenSide side, wxSize size) {
  return new ScreenText(parent, initial_text, side, size);
}

ScreenText* ScreenText::getPresenter(wxWindow* parent, ScreenText* preview,
                                     wxSize size) {
  // Note that we copy most elements to the presenter, but specifically not the
  // size.
  return new ScreenText(parent, preview->texts, preview->image,
                        preview->background_color, preview->screen_side, size);
}

ScreenText::ScreenText(wxWindow* parent, std::vector<RenderableText> texts,
                       wxImage image, std::optional<Color> background_color,
                       proto::ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  for (auto new_text : texts) {
    this->texts.push_back(new_text);
  }
  this->image = image;
  this->screen_side = side;
  this->background_color = background_color;
  if (background_color.has_value()) {
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

ScreenText::ScreenText(wxWindow* parent, const wxString& initial_text,
                       proto::ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  RenderableText default_text(initial_text, 10);
  texts.push_back(default_text);

  this->screen_side = side;

  if (side.error()) {
    image = BackgroundImage::errorImage(size);
    texts[0].setFontColor(Color("Black"));
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

void ScreenText::resetAllText() { texts.clear(); }

void ScreenText::addText(const wxString& text, proto::Font font,
                         const proto::ScreenSide& side) {
  if (isSide(side)) {
    RenderableText renderable_text(text, font);
    renderable_text.setFontColor(background_color->contrastColor());
    texts.push_back(renderable_text);
  }
}

void ScreenText::setText(const wxString& text, int font_size,
                         const proto::ScreenSide& side) {
  if (isSide(side)) {
    resetAllText();
    proto::Font font;
    font.set_size(font_size);
    addText(text, font, side);
  }
}

void ScreenText::initializeForColor(wxSize size, Color color) {
  image = BackgroundImage(size, color);
  for (auto &text : texts) {
    text.setFontColor(color.contrastColor());
  }
  if (!blackout_image.IsOk() ||
      size.GetWidth() != blackout_image.GetSize().GetWidth() ||
      size.GetHeight() != blackout_image.GetSize().GetHeight()) {
    blackout_image = BackgroundImage(size, Color("Black"));
  }
}

void ScreenText::blackout() {
  background_color = Color("Black");
  image = blackout_image;
  resetAllText();
  Refresh();
}

void ScreenText::renderBackground(wxDC& dc) {
  dc.DrawBitmap(wxBitmap(image, 32), 0, 0, false);
}

void ScreenText::renderText(wxDC& dc, RenderableText text) {
  dc.SetFont(ProtoUtil::wxScaledFont(text.getFont(), GetSize()));
  dc.SetTextForeground(ProtoUtil::wxClr(text.getFont().color()));
  int width, height;
  getTextExtent(dc, text.getText(), &width, &height);
  int x = (GetSize().GetWidth() - width) / 2;
  int y = (GetSize().GetHeight() - height) / 2;
  dc.DrawText(text.getText(), x, y);
}

void ScreenText::renderAllText(wxDC& dc) {
  for (auto text : texts) {
    renderText(dc, text);
  }
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

void ScreenText::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  renderBackground(dc);
  renderAllText(dc);
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

  resetAllText();
  for (auto new_text : source.texts) {
    addText(new_text.getText(), new_text.getFont(), this->screen_side);
  }

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

RenderableText::RenderableText(wxString text, float font_size) {
  this->text = text;
  setFont(font_size);
  font.set_style(proto::Font_FontStyle_FONT_STYLE_IMPACT);
}

RenderableText::RenderableText(wxString text, proto::Font font) {
  this->text = text;
  this->font = font;

  if (this->font.style() == proto::Font_FontStyle_FONT_STYLE_UNDEFINED) {
    this->font.set_style(proto::Font_FontStyle_FONT_STYLE_IMPACT);
  }
}

void RenderableText::setFont(float font_size) { font.set_size(font_size); }

void RenderableText::setFontColor(Color color) {
  ProtoUtil::protoClr(color, font.mutable_color());
}

void RenderableText::setText(const wxString& text, int font_size) {
  this->text = text;
  setFont(font_size);
}

wxString RenderableText::getText() { return text; }

proto::Font RenderableText::getFont() { return font; }

}  // namespace cszb_scoreboard
