/*
ui/component/ScreenTextSide.cpp: A single side of renderable image/text.  One or
more of these are contained in a ScreenText, for use in presenting to a preview
or external monitor.

Copyright 2019-2020 Tracy Beck

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

#include "ui/component/ScreenTextSide.h"

#include <wx/image.h>
#include <wx/tokenzr.h>

#include "config/TeamConfig.h"
#include "ui/graphics/BackgroundImage.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

// Margin for the top or bottom, as a percentage
const float TOP_OR_BOTTOM_MARGIN = 2;

ScreenTextSide::ScreenTextSide(wxWindow* parent, ScreenTextSide* source_side,
                               wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  for (auto new_text : source_side->texts) {
    this->texts.push_back(new_text);
  }
  this->image = source_side->image;
  this->screen_side = source_side->screen_side;
  this->background_color = source_side->background_color;
  image_is_scaled = false;
  if (background_color.has_value()) {
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

ScreenTextSide::ScreenTextSide(wxWindow* parent, const wxString& initial_text,
                               proto::ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  proto::RenderableText default_text;
  default_text.set_text(initial_text);
  ProtoUtil::defaultFont(default_text.mutable_font());
  texts.push_back(default_text);

  this->screen_side = side;
  image_is_scaled = false;

  if (side.error()) {
    image = BackgroundImage::errorImage(size);
    ProtoUtil::setFontColor(texts[0].mutable_font(), Color("Black"));
  } else {
    setDefaultBackground(side);
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

void ScreenTextSide::bindEvents() {
  Bind(wxEVT_PAINT, &ScreenTextSide::paintEvent, this);
}

void ScreenTextSide::resetAllText(const proto::ScreenSide& side) {
  if (isSide(side)) {
    texts.clear();
  }
}

void ScreenTextSide::setAutoFit(bool auto_fit, const proto::ScreenSide& side) {
  if (isSide(side)) {
    auto_fit_text = auto_fit;
  }
}

void ScreenTextSide::addText(proto::RenderableText text,
                             const proto::ScreenSide& side) {
  if (isSide(side)) {
    texts.push_back(text);
  }
}

void ScreenTextSide::setText(const wxString& text, int font_size,
                             const proto::ScreenSide& side) {
  // We don't really need to check side in this method, but it saves us a lot of
  // renderable_text creation if we do.
  if (isSide(side)) {
    resetAllText(side);

    proto::RenderableText renderable_text;
    renderable_text.set_text(text);
    renderable_text.mutable_font()->set_size(font_size);

    ProtoUtil::validateFont(renderable_text.mutable_font());
    setFontColor(renderable_text.mutable_font());

    addText(renderable_text, side);
  }
}

void ScreenTextSide::initializeForColor(wxSize size, Color color) {
  image_is_scaled = false;
  image = BackgroundImage(size, color);
  for (auto& text : texts) {
    ProtoUtil::setFontColor(text.mutable_font(), color.contrastColor());
  }
  createBlackout();
}

void ScreenTextSide::createBlackout() {
  if (!blackout_image.IsOk() ||
      GetSize().GetWidth() != blackout_image.GetSize().GetWidth() ||
      GetSize().GetHeight() != blackout_image.GetSize().GetHeight()) {
    blackout_image = BackgroundImage(GetSize(), Color("Black"));
  }
}

void ScreenTextSide::setFontColor(proto::Font* font) {
  if (background_color.has_value()) {
    ProtoUtil::setFontColor(font, background_color->contrastColor());
  } else {
    // Just assume white as a default for now.
    ProtoUtil::setFontColor(font, Color("White"));
  }
}

void ScreenTextSide::setSize(const wxSize& size) {
  if (size == GetSize()) {
    return;
  }
  SetSize(size);
  if (background_color.has_value()) {
    initializeForColor(GetSize(), *background_color);
  }
  createBlackout();
}

void ScreenTextSide::blackout() {
  background_color = Color("Black");
  image = blackout_image;
  resetAllText(this->screen_side);
  Refresh();
}

void ScreenTextSide::renderScaledBackground(wxDC& dc) {
  wxImage scaled_image = image;
  wxSize screen_size = GetSize();
  wxSize image_size = scaled_image.GetSize();
  float screen_ratio = ratio(screen_size);
  float image_ratio = ratio(image_size);
  int image_height, image_width;
  if (screen_ratio > image_ratio) {
    // Screen is wider than image, so make the heights match
    image_height = screen_size.GetHeight();
    image_width = screen_size.GetHeight() * image_ratio;
  } else {
    // Screen is either the same ratio or narrower than image, so make the
    // widths match
    image_width = screen_size.GetWidth();
    image_height = screen_size.GetWidth() / image_ratio;
  }

  scaled_image.Rescale(image_width, image_height);
  int x = (screen_size.GetWidth() - image_width) / 2;
  int y = (screen_size.GetHeight() - image_height) / 2;

  dc.DrawBitmap(wxBitmap(scaled_image, 32), x, y, false);
}

void ScreenTextSide::renderBackground(wxDC& dc) {
  if (image_is_scaled) {
    renderScaledBackground(dc);
  } else {
    dc.DrawBitmap(wxBitmap(image, 32), 0, 0, false);
  }
}

void ScreenTextSide::autoFitText(wxDC& dc, proto::RenderableText& text) {
  proto::Font* font = text.mutable_font();
  wxSize screen_size = GetSize();

  dc.SetFont(ProtoUtil::wxScaledFont(*font, screen_size));
  wxSize text_extent = getTextExtent(dc, text.text());

  while (text_extent.GetWidth() > screen_size.GetWidth() && font->size() > 0) {
    font->set_size(font->size() - .5);
    dc.SetFont(ProtoUtil::wxScaledFont(*font, screen_size));
    text_extent = getTextExtent(dc, text.text());
  }

  while (text_extent.GetHeight() > screen_size.GetHeight() &&
         font->size() > 0) {
    font->set_size(font->size() - .5);
    dc.SetFont(ProtoUtil::wxScaledFont(*font, screen_size));
    text_extent = getTextExtent(dc, text.text());
  }
}

wxPoint ScreenTextSide::bottomText(wxDC& dc, wxString text) {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int margin = GetSize().GetHeight() * (TOP_OR_BOTTOM_MARGIN / 100);
  int y = GetSize().GetHeight() - text_extent.GetHeight() - margin;
  return wxPoint(x, y);
}

wxPoint ScreenTextSide::centerText(wxDC& dc, wxString text) {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int y = (GetSize().GetHeight() - text_extent.GetHeight()) / 2;
  return wxPoint(x, y);
}

wxPoint ScreenTextSide::topText(wxDC& dc, wxString text) {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int y = GetSize().GetHeight() * (TOP_OR_BOTTOM_MARGIN / 100);
  return wxPoint(x, y);
}

wxPoint ScreenTextSide::positionText(wxDC& dc, proto::RenderableText text) {
  switch (text.position()) {
    case proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_BOTTOM:
      return bottomText(dc, text.text());
      break;
    case proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP:
      return topText(dc, text.text());
      break;
    case proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_CENTERED:
    default:
      return centerText(dc, text.text());
      break;
  }
}

void ScreenTextSide::renderText(wxDC& dc, proto::RenderableText& text) {
  if (auto_fit_text) {
    autoFitText(dc, text);
  }
  dc.SetFont(ProtoUtil::wxScaledFont(text.font(), GetSize()));
  dc.SetTextForeground(ProtoUtil::wxClr(text.font().color()));
  wxPoint placement = positionText(dc, text);
  dc.DrawText(text.text(), placement.x, placement.y);
}

void ScreenTextSide::renderAllText(wxDC& dc) {
  for (auto& text : texts) {
    renderText(dc, text);
  }
}

wxSize ScreenTextSide::getTextExtent(wxDC& dc, wxString text) {
  wxStringTokenizer tokens(text, "\n\r");
  int width = 0;
  int height = 0;
  wxString token = tokens.GetNextToken();
  while (!token.IsEmpty()) {
    int line_width, line_height;
    dc.GetTextExtent(token, &line_width, &line_height);
    if (line_width > width) width = line_width;
    height += line_height;
    token = tokens.GetNextToken();
  }

  return wxSize(width, height);
}

void ScreenTextSide::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  renderBackground(dc);
  renderAllText(dc);
}

void ScreenTextSide::setImage(const wxImage& image) {
  background_color.reset();
  this->image = image;
}

void ScreenTextSide::setImage(const wxImage& image, bool is_scaled,
                              const proto::ScreenSide& side) {
  if (isSide(side)) {
    image_is_scaled = is_scaled;
    setImage(image);
  }
};

void ScreenTextSide::setBackground(const Color& color,
                                   const proto::ScreenSide& side) {
  if (isSide(side)) {
    setBackground(color);
  }
}

void ScreenTextSide::setBackground(const Color& color) {
  this->background_color = color;
  initializeForColor(GetSize(), color);
};

void ScreenTextSide::setDefaultBackground(const proto::ScreenSide& side) {
  // TODO: Allow for a view to contain multiple sides
  Color background_color = TeamConfig::getInstance()->teamColor(side)[0];
  setBackground(background_color, side);
}

void ScreenTextSide::setAll(const ScreenTextSide* source) {
  setImage(source->image, source->image_is_scaled, this->screen_side);
  if (source->background_color.has_value()) {
    setBackground(*source->background_color);
  } else {
    background_color.reset();
  }

  resetAllText(this->screen_side);
  for (auto new_text : source->texts) {
    addText(new_text, this->screen_side);
  }

  Refresh();
}

bool ScreenTextSide::isSide(proto::ScreenSide side) {
  if (screen_side.home() && side.home()) {
    return true;
  } else if (screen_side.away() && side.away()) {
    return true;
  } else if (screen_side.extra() && side.extra()) {
    return true;
  }
  return false;
}

float ScreenTextSide::ratio(const wxSize& size) {
  float ratio = 4 / 3;
  ratio = (float)size.GetWidth() / size.GetHeight();
  return ratio;
}

}  // namespace cszb_scoreboard
