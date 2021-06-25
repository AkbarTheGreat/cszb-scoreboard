/*
ui/component/ScreenTextSide.cpp: A single side of renderable image/text.  One or
more of these are contained in a ScreenText, for use in presenting to a preview
or external monitor.

Copyright 2019-2021 Tracy Beck

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
const float TOP_OR_BOTTOM_MARGIN = 2.0F;
const float TOP_OR_BOTTOM_RATIO = TOP_OR_BOTTOM_MARGIN / 100;
const float BOTTOM_CORNER_OVERLAY_SCALE = 0.30F;
const float AUTOFIT_FONT_ADJUSTMENT = 0.5F;

ScreenTextSide::ScreenTextSide(wxWindow* parent, ScreenTextSide* source_side,
                               wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {
  for (const auto& new_text : source_side->texts) {
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
                               const proto::ScreenSide& side, wxSize size)
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

void ScreenTextSide::addText(const proto::RenderableText& text,
                             const proto::ScreenSide& side) {
  if (isSide(side)) {
    proto::RenderableText mutable_text(text);
    ProtoUtil::validateFont(mutable_text.mutable_font());
    setFontColor(mutable_text.mutable_font(), side);
    texts.push_back(mutable_text);
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
    setFontColor(renderable_text.mutable_font(), side);

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

void ScreenTextSide::setFontColor(proto::Font* font,
                                  const proto::ScreenSide& side) {
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

void ScreenTextSide::renderScaledBackground(wxDC* dc) {
  wxImage scaled_image = scaleImage(image, GetSize());
  int x = (GetSize().GetWidth() - scaled_image.GetSize().GetWidth()) / 2;
  int y = (GetSize().GetHeight() - scaled_image.GetSize().GetHeight()) / 2;
  dc->DrawBitmap(wxBitmap(scaled_image), x, y, false);
}

void ScreenTextSide::renderOverlay(wxDC* dc) {
  if (!background_overlay.has_value()) {
    return;
  }

  if (overlay_position == OverlayScreenPosition::Centered) {
    renderOverlayCentered(dc);
  } else if (overlay_position == OverlayScreenPosition::BottomLeft) {
    renderOverlayBottomCorner(dc);
  }
}

void ScreenTextSide::renderOverlayBottomCorner(wxDC* dc) {
  wxImage scaled_image =
      scaleImage(*background_overlay, GetSize() * BOTTOM_CORNER_OVERLAY_SCALE);
  Color font_color("White");
  if (!texts.empty()) {
    font_color = ProtoUtil::wxClr(texts[0].font().color());
  }
  adjustOverlayColorAndAlpha(&scaled_image, font_color);

  int x = TOP_OR_BOTTOM_MARGIN;
  int y = GetSize().GetHeight() - scaled_image.GetSize().GetHeight() -
          TOP_OR_BOTTOM_MARGIN;
  dc->DrawBitmap(wxBitmap(scaled_image), x, y, false);
}

void ScreenTextSide::renderOverlayCentered(wxDC* dc) {
  wxImage scaled_image =
      scaleImage(*background_overlay, GetSize() * overlay_percentage);

  Color font_color("White");
  if (!texts.empty()) {
    font_color = ProtoUtil::wxClr(texts[0].font().color());
  }
  adjustOverlayColorAndAlpha(&scaled_image, font_color);

  int x = (GetSize().GetWidth() - scaled_image.GetSize().GetWidth()) / 2;
  int y = (GetSize().GetHeight() - scaled_image.GetSize().GetHeight()) / 2;
  dc->DrawBitmap(wxBitmap(scaled_image), x, y, false);
}

void ScreenTextSide::renderBackground(wxDC* dc) {
  if (image_is_scaled) {
    renderScaledBackground(dc);
  } else {
    dc->DrawBitmap(wxBitmap(image), 0, 0, false);
  }
  renderOverlay(dc);
}

auto ScreenTextSide::scaleImage(const wxImage& image, const wxSize& target_size)
    -> wxImage {
  wxImage scaled_image = image;
  wxSize image_size = scaled_image.GetSize();
  float screen_ratio = ratio(target_size);
  float image_ratio = ratio(image_size);
  int image_height;
  int image_width;
  if (screen_ratio > image_ratio) {
    // Screen is wider than image, so make the heights match
    image_height = target_size.GetHeight();
    image_width = target_size.GetHeight() * image_ratio;
  } else {
    // Screen is either the same ratio or narrower than image, so make the
    // widths match
    image_width = target_size.GetWidth();
    image_height = target_size.GetWidth() / image_ratio;
  }

  scaled_image.Rescale(image_width, image_height);
  return scaled_image;
}

void ScreenTextSide::adjustOverlayColorAndAlpha(wxImage* image,
                                                const Color& color) const {
  // We presume that the overlay is predominantly black, so subtracting it from
  // the font color should give us that color most of the time.
  wxRect dimensions(wxPoint(0, 0), image->GetSize());
  image->SetRGB(dimensions, color.Red(), color.Green(), color.Blue());
  unsigned char* alpha = image->GetAlpha();
  for (int i = 0;
       i < image->GetSize().GetWidth() * image->GetSize().GetHeight(); i++) {
    if (alpha[i] > overlay_alpha) {
      alpha[i] = overlay_alpha;
    }
  }
}

void ScreenTextSide::autoFitText(wxDC* dc, proto::RenderableText* text) {
  proto::Font* font = text->mutable_font();
  wxSize screen_size = GetSize();

  dc->SetFont(ProtoUtil::wxScaledFont(*font, screen_size));
  wxSize text_extent = getTextExtent(dc, text->text());

  while (text_extent.GetWidth() > screen_size.GetWidth() && font->size() > 0) {
    font->set_size(font->size() - AUTOFIT_FONT_ADJUSTMENT);
    dc->SetFont(ProtoUtil::wxScaledFont(*font, screen_size));
    text_extent = getTextExtent(dc, text->text());
  }

  while (text_extent.GetHeight() > screen_size.GetHeight() &&
         font->size() > 0) {
    font->set_size(font->size() - AUTOFIT_FONT_ADJUSTMENT);
    dc->SetFont(ProtoUtil::wxScaledFont(*font, screen_size));
    text_extent = getTextExtent(dc, text->text());
  }
}

auto ScreenTextSide::bottomText(wxDC* dc, const wxString& text) -> wxPoint {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int margin = GetSize().GetHeight() * TOP_OR_BOTTOM_RATIO;
  int y = GetSize().GetHeight() - text_extent.GetHeight() - margin;
  return wxPoint(x, y);
}

auto ScreenTextSide::centerText(wxDC* dc, const wxString& text) -> wxPoint {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int y = (GetSize().GetHeight() - text_extent.GetHeight()) / 2;
  return wxPoint(x, y);
}

auto ScreenTextSide::topText(wxDC* dc, const wxString& text) -> wxPoint {
  wxSize text_extent = getTextExtent(dc, text);
  int x = (GetSize().GetWidth() - text_extent.GetWidth()) / 2;
  int y = GetSize().GetHeight() * TOP_OR_BOTTOM_RATIO;
  return wxPoint(x, y);
}

auto ScreenTextSide::positionText(wxDC* dc, const proto::RenderableText& text)
    -> wxPoint {
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

void ScreenTextSide::renderText(wxDC* dc, proto::RenderableText* text) {
  if (auto_fit_text) {
    autoFitText(dc, text);
  }
  dc->SetFont(ProtoUtil::wxScaledFont(text->font(), GetSize()));
  dc->SetTextForeground(ProtoUtil::wxClr(text->font().color()));
  wxPoint placement = positionText(dc, *text);
  dc->DrawText(text->text(), placement.x, placement.y);
}

void ScreenTextSide::renderAllText(wxDC* dc) {
  for (auto& text : texts) {
    renderText(dc, &text);
  }
}

auto ScreenTextSide::getTextExtent(wxDC* dc, const wxString& text) -> wxSize {
  wxStringTokenizer tokens(text, "\n\r", wxTOKEN_RET_EMPTY_ALL);
  int width = 0;
  int height = 0;
  while (tokens.HasMoreTokens()) {
    wxString token = tokens.GetNextToken();
    if (token.IsEmpty()) {
      // If the line is empty, the vertical extent reads as 0, when it should be
      // the constant height of a character.  So put a thin character here to
      // get the correct vertical extent.
      token = "|";
    }
    int line_width;
    int line_height;
    dc->GetTextExtent(token, &line_width, &line_height);
    if (line_width > width) {
      width = line_width;
    }
    height += line_height;
  }

  return wxSize(width, height);
}

void ScreenTextSide::paintEvent(wxPaintEvent& event) {
  wxPaintDC dc(this);
  renderBackground(&dc);
  renderAllText(&dc);
}

void ScreenTextSide::setImage(const wxImage& image, bool is_scaled,
                              const proto::ScreenSide& side) {
  if (isSide(side)) {
    image_is_scaled = is_scaled;
    background_color.reset();
    this->image = image;
    background_overlay.reset();
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
  background_overlay.reset();
};

void ScreenTextSide::setBackgroundOverlay(const wxImage& overlay,
                                          double overlay_screen_percentage,
                                          unsigned char overlay_alpha,
                                          OverlayScreenPosition position,
                                          const proto::ScreenSide& side) {
  if (isSide(side)) {
    background_overlay = overlay;
    overlay_percentage = overlay_screen_percentage;
    overlay_position = position;
    this->overlay_alpha = overlay_alpha;
  }
}

void ScreenTextSide::setDefaultBackground(const proto::ScreenSide& side) {
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

  if (source->background_overlay.has_value()) {
    background_overlay = *source->background_overlay;
    overlay_alpha = source->overlay_alpha;
    overlay_percentage = source->overlay_percentage;
    overlay_position = source->overlay_position;
  } else {
    background_overlay.reset();
  }

  resetAllText(this->screen_side);
  for (const auto& new_text : source->texts) {
    addText(new_text, this->screen_side);
  }

  Refresh();
}

auto ScreenTextSide::isSide(const proto::ScreenSide& side) -> bool {
  if (screen_side.home() && side.home()) {
    return true;
  }
  if (screen_side.away() && side.away()) {
    return true;
  }
  return (screen_side.extra() && side.extra());
}

auto ScreenTextSide::ratio(const wxSize& size) -> float {
  float ratio = 4 / 3;
  ratio = static_cast<float>(size.GetWidth()) / size.GetHeight();
  return ratio;
}

}  // namespace cszb_scoreboard
