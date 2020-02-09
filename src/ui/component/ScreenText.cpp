/*
ui/component/ScreenText.cpp: Represents a text presentation on a ScreenPresenter
or a ScreenPreview.

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

#include "ui/component/ScreenText.h"

#include <wx/image.h>
#include <wx/tokenzr.h>

#include "config/TeamConfig.h"
#include "ui/graphics/BackgroundImage.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

// Margin for the top or bottom, as a percentage
const float TOP_OR_BOTTOM_MARGIN = 2;

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

ScreenText::ScreenText(wxWindow* parent,
                       std::vector<proto::RenderableText> texts, wxImage image,
                       std::optional<Color> background_color,
                       proto::ScreenSide side, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  for (auto new_text : texts) {
    this->texts.push_back(new_text);
  }
  this->image = image;
  this->screen_side = side;
  this->background_color = background_color;
  image_is_scaled = false;
  if (background_color.has_value()) {
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

ScreenText::ScreenText(wxWindow* parent, const wxString& initial_text,
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

void ScreenText::resetAllText(const proto::ScreenSide& side) {
  if (isSide(side)) {
    texts.clear();
  }
}

void ScreenText::addText(proto::RenderableText text,
                         const proto::ScreenSide& side) {
  if (isSide(side)) {
    texts.push_back(text);
  }
}

void ScreenText::setText(const wxString& text, int font_size,
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

void ScreenText::initializeForColor(wxSize size, Color color) {
  image_is_scaled = false;
  image = BackgroundImage(size, color);
  for (auto& text : texts) {
    ProtoUtil::setFontColor(text.mutable_font(), color.contrastColor());
  }
  if (!blackout_image.IsOk() ||
      size.GetWidth() != blackout_image.GetSize().GetWidth() ||
      size.GetHeight() != blackout_image.GetSize().GetHeight()) {
    blackout_image = BackgroundImage(size, Color("Black"));
  }
}

void ScreenText::setFontColor(proto::Font* font) {
  if (background_color.has_value()) {
    ProtoUtil::setFontColor(font, background_color->contrastColor());
  } else {
    // Just assume white as a default for now.
    ProtoUtil::setFontColor(font, Color("White"));
  }
}

void ScreenText::blackout() {
  background_color = Color("Black");
  image = blackout_image;
  resetAllText(this->screen_side);
  Refresh();
}

void ScreenText::renderScaledBackground(wxDC& dc) {
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

void ScreenText::renderBackground(wxDC& dc) {
  if (image_is_scaled) {
    renderScaledBackground(dc);
  } else {
    dc.DrawBitmap(wxBitmap(image, 32), 0, 0, false);
  }
}

wxPoint ScreenText::bottomText(wxDC& dc, wxString text) {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int margin = GetSize().GetHeight() * (TOP_OR_BOTTOM_MARGIN / 100);
  int y = GetSize().GetHeight() - text_extent.GetHeight() - margin;
  return wxPoint(x, y);
}

wxPoint ScreenText::centerText(wxDC& dc, wxString text) {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int y = (GetSize().GetHeight() - text_extent.GetHeight()) / 2;
  return wxPoint(x, y);
}

wxPoint ScreenText::topText(wxDC& dc, wxString text) {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int y = GetSize().GetHeight() * (TOP_OR_BOTTOM_MARGIN / 100);
  return wxPoint(x, y);
}

wxPoint ScreenText::positionText(wxDC& dc, proto::RenderableText text) {
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

void ScreenText::renderText(wxDC& dc, proto::RenderableText text) {
  dc.SetFont(ProtoUtil::wxScaledFont(text.font(), GetSize()));
  dc.SetTextForeground(ProtoUtil::wxClr(text.font().color()));
  wxPoint placement = positionText(dc, text);
  dc.DrawText(text.text(), placement.x, placement.y);
}

void ScreenText::renderAllText(wxDC& dc) {
  for (auto text : texts) {
    renderText(dc, text);
  }
}

wxSize ScreenText::getTextExtent(wxDC& dc, wxString text) {
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

void ScreenText::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  renderBackground(dc);
  renderAllText(dc);
}

void ScreenText::setImage(const wxImage& image) {
  background_color.reset();
  this->image = image;
}

void ScreenText::setImage(const wxImage& image, bool is_scaled) {
  image_is_scaled = is_scaled;
  setImage(image);
};

void ScreenText::setBackground(const Color& color,
                               const proto::ScreenSide& side) {
  if (isSide(side)) {
    setBackground(color);
  }
}

void ScreenText::setBackground(const Color& color) {
  this->background_color = color;
  initializeForColor(GetSize(), color);
};

void ScreenText::setAll(const ScreenText& source) {
  setImage(source.image, source.image_is_scaled);
  if (source.background_color.has_value()) {
    setBackground(*source.background_color);
  } else {
    background_color.reset();
  }

  resetAllText(this->screen_side);
  for (auto new_text : source.texts) {
    addText(new_text, this->screen_side);
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

float ScreenText::ratio(const wxSize& size) {
  float ratio = 4 / 3;
  ratio = (float)size.GetWidth() / size.GetHeight();
  return ratio;
}

}  // namespace cszb_scoreboard
