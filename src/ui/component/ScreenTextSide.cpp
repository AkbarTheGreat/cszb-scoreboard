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

#include <wx/gdicmn.h>  // for wxSize

#include <memory>  // for allocator_traits<>::value_type

#include "config/Position.h"              // for Size, Position
#include "config/TeamConfig.h"            // for TeamConfig
#include "config/swx/event.h"             // for wxEVT_PAINT
#include "ui/graphics/BackgroundImage.h"  // for BackgroundImage
#include "ui/widget/RenderContext.h"      // for RenderContext
#include "util/ProtoUtil.h"               // for ProtoUtil

// Temporary, probably -- used as I prove out the timer stuff.
#include <util/StringUtil.h>

#include <chrono>

namespace cszb_scoreboard {

// Margin for the top or bottom, as a percentage
constexpr float TOP_OR_BOTTOM_MARGIN = 2.0F;
constexpr float TOP_OR_BOTTOM_RATIO = TOP_OR_BOTTOM_MARGIN / 100;
constexpr float BOTTOM_CORNER_OVERLAY_SCALE = 0.30F;
constexpr float AUTOFIT_FONT_ADJUSTMENT = 0.5F;
constexpr float TIMER_BACKGROUND_PCT = .25;
constexpr float TIMER_FONT_SIZE = 10;
constexpr int TIMER_ALPHA = 128;

ScreenTextSide::ScreenTextSide(swx::Panel *wx, ScreenTextSide *source_side,
                               Size size, Singleton *singleton)
    : ScreenTextSide(singleton, wx, source_side->screen_side) {
  for (const auto &new_text : source_side->texts) {
    this->texts.push_back(new_text);
  }
  this->image = source_side->image;
  this->background_color = source_side->background_color;
  if (background_color.has_value()) {
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

ScreenTextSide::ScreenTextSide(swx::Panel *wx, const std::string &initial_text,
                               const proto::ScreenSide &side, Size size,
                               Singleton *singleton)
    : ScreenTextSide(singleton, wx, side) {
  proto::RenderableText default_text;
  default_text.set_text(initial_text);
  ProtoUtil::defaultFont(default_text.mutable_font());
  texts.push_back(default_text);

  if (side.error()) {
    image = BackgroundImage::errorImage(size);
    ProtoUtil::setFontColor(texts[0].mutable_font(), Color("Black"));
  } else {
    setDefaultBackground(side);
    initializeForColor(size, *background_color);
  }
  bindEvents();
}

ScreenTextSide::ScreenTextSide(Singleton *singleton, swx::Panel *wx,
                               const proto::ScreenSide &side)
    : Panel(wx) {
  this->image_is_scaled = false;
  this->screen_side = side;
  this->singleton = singleton;
}

void ScreenTextSide::bindEvents() {
  bind(wxEVT_PAINT,
       [this](RenderContext *renderer) -> void { this->paintEvent(renderer); });
}

void ScreenTextSide::resetAllText(const proto::ScreenSide &side) {
  if (isSide(side)) {
    texts.clear();
  }
}

void ScreenTextSide::setAutoFit(bool auto_fit, const proto::ScreenSide &side) {
  if (isSide(side)) {
    auto_fit_text = auto_fit;
  }
}

void ScreenTextSide::addText(const proto::RenderableText &text,
                             const proto::ScreenSide &side) {
  if (isSide(side)) {
    proto::RenderableText mutable_text(text);
    ProtoUtil::validateFont(mutable_text.mutable_font());
    setFontColor(mutable_text.mutable_font(), side);
    texts.push_back(mutable_text);
  }
}

void ScreenTextSide::setText(const std::string &text, int font_size,
                             const proto::ScreenSide &side) {
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

void ScreenTextSide::initializeForColor(Size size, const Color &color) {
  image_is_scaled = false;
  image = BackgroundImage(size, color);
  for (auto &text : texts) {
    ProtoUtil::setFontColor(text.mutable_font(), color.contrastColor());
  }
  createBlackout();
}

void ScreenTextSide::createBlackout() {
  if (!blackout_image.IsOk() || size() != blackout_image.size()) {
    blackout_image = BackgroundImage(size(), Color("Black"));
  }
}

void ScreenTextSide::setFontColor(proto::Font *font,
                                  const proto::ScreenSide &side) {
  if (background_color.has_value()) {
    ProtoUtil::setFontColor(font, background_color->contrastColor());
  } else {
    // Just assume white as a default for now.
    ProtoUtil::setFontColor(font, Color("White"));
  }
}

void ScreenTextSide::setSize(const Size &size) {
  if (size == this->size()) {
    return;
  }
  Panel::setSize(size);
  if (background_color.has_value()) {
    initializeForColor(this->size(), *background_color);
  }
  createBlackout();
}

void ScreenTextSide::blackout() {
  background_color = Color("Black");
  image = blackout_image;
  resetAllText(screen_side);
  refresh();
}

void ScreenTextSide::renderScaledBackground(RenderContext *renderer) {
  Image scaled_image = scaleImage(image, size());
  int x = (size().width - scaled_image.size().width) / 2;
  int y = (size().height - scaled_image.size().height) / 2;
  renderer->drawImage(scaled_image, x, y);
}

void ScreenTextSide::renderOverlay(RenderContext *renderer) {
  if (!background_overlay.has_value()) {
    return;
  }

  if (overlay_position == OverlayScreenPosition::Centered) {
    renderOverlayCentered(renderer);
  } else if (overlay_position == OverlayScreenPosition::BottomLeft) {
    renderOverlayBottomCorner(renderer);
  }
}

void ScreenTextSide::renderOverlayBottomCorner(RenderContext *renderer) {
  Image scaled_image =
      scaleImage(*background_overlay, size() * BOTTOM_CORNER_OVERLAY_SCALE);
  Color font_color("White");
  if (!texts.empty()) {
    font_color = ProtoUtil::wxClr(texts[0].font().color());
  }
  adjustOverlayColorAndAlpha(&scaled_image, font_color);

  int x = TOP_OR_BOTTOM_MARGIN;
  int y = size().height - scaled_image.size().height - TOP_OR_BOTTOM_MARGIN;
  renderer->drawImage(scaled_image, x, y);
}

void ScreenTextSide::renderOverlayCentered(RenderContext *renderer) {
  Image scaled_image =
      scaleImage(*background_overlay, size() * overlay_percentage);

  Color font_color("White");
  if (!texts.empty()) {
    font_color = ProtoUtil::wxClr(texts[0].font().color());
  }
  adjustOverlayColorAndAlpha(&scaled_image, font_color);

  int x = (size().width - scaled_image.size().width) / 2;
  int y = (size().height - scaled_image.size().height) / 2;
  renderer->drawImage(scaled_image, x, y);
}

void ScreenTextSide::renderBackground(RenderContext *renderer) {
  if (image_is_scaled) {
    renderScaledBackground(renderer);
  } else {
    renderer->drawImage(image, 0, 0);
  }
  renderOverlay(renderer);
}

auto ScreenTextSide::scaleImage(const Image &image, const Size &target_size)
    -> Image {
  Image scaled_image = image;
  Size image_size = scaled_image.size();
  float screen_ratio = ratio(target_size);
  float image_ratio = ratio(image_size);
  int image_height;
  int image_width;
  if (screen_ratio > image_ratio) {
    // Screen is wider than image, so make the heights match
    image_height = target_size.height;
    image_width = target_size.height * image_ratio;
  } else {
    // Screen is either the same ratio or narrower than image, so make the
    // widths match
    image_width = target_size.width;
    image_height = target_size.width / image_ratio;
  }

  scaled_image.Rescale(image_width, image_height);
  return scaled_image;
}

void ScreenTextSide::adjustOverlayColorAndAlpha(Image *image,
                                                const Color &color) const {
  // We presume that the overlay is predominantly black, so subtracting it from
  // the font color should give us that color most of the time.
  image->setColor(color);
  unsigned char *alpha = image->GetAlpha();
  for (int i = 0;
       i < image->GetSize().GetWidth() * image->GetSize().GetHeight(); i++) {
    if (alpha[i] > overlay_alpha) {
      alpha[i] = overlay_alpha;
    }
  }
}

void ScreenTextSide::autoFitText(RenderContext *renderer,
                                 proto::RenderableText *text) {
  proto::Font *font = text->mutable_font();

  renderer->setFont(*font, size());
  Size text_extent = getTextExtent(renderer, text->text());

  while (text_extent.width > size().width && font->size() > 0) {
    font->set_size(font->size() - AUTOFIT_FONT_ADJUSTMENT);
    renderer->setFont(*font, size());
    text_extent = getTextExtent(renderer, text->text());
  }

  while (text_extent.height > size().height && font->size() > 0) {
    font->set_size(font->size() - AUTOFIT_FONT_ADJUSTMENT);
    renderer->setFont(*font, size());
    text_extent = getTextExtent(renderer, text->text());
  }
}

auto ScreenTextSide::bottomText(RenderContext *renderer,
                                const std::string &text) -> Position {
  Size text_extent = getTextExtent(renderer, text);
  int x = (size().width - text_extent.width) / 2;
  int margin = size().height * TOP_OR_BOTTOM_RATIO;
  int y = size().height - text_extent.height - margin;
  return Position{.x = x, .y = y};
}

auto ScreenTextSide::centerText(RenderContext *renderer,
                                const std::string &text) -> Position {
  Size text_extent = getTextExtent(renderer, text);
  int x = (size().width - text_extent.width) / 2;
  int y = (size().height - text_extent.height) / 2;
  return Position{.x = x, .y = y};
}

auto ScreenTextSide::topText(RenderContext *renderer, const std::string &text)
    -> Position {
  Size text_extent = getTextExtent(renderer, text);
  int x = (size().width - text_extent.width) / 2;
  int y = size().height * TOP_OR_BOTTOM_RATIO;
  return Position{.x = x, .y = y};
}

auto ScreenTextSide::positionText(RenderContext *renderer,
                                  const proto::RenderableText &text)
    -> Position {
  switch (text.position()) {
    case proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_BOTTOM:
      return bottomText(renderer, text.text());
      break;
    case proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_TOP:
      return topText(renderer, text.text());
      break;
    case proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_CENTERED:
    default:
      return centerText(renderer, text.text());
      break;
  }
}

void ScreenTextSide::renderText(RenderContext *renderer,
                                proto::RenderableText *text) {
  if (auto_fit_text) {
    autoFitText(renderer, text);
  }
  renderer->setFont(text->font(), size());
  renderer->setTextColor(ProtoUtil::wxClr(text->font().color()));
  Position placement = positionText(renderer, *text);
  renderer->drawText(text->text(), placement.x, placement.y);
}

void ScreenTextSide::renderTimer(RenderContext *renderer) {
  proto::RenderableText timer_text;
  timer_text.set_text("00:00");
  timer_text.set_position(
      proto::RenderableText_ScreenPosition_FONT_SCREEN_POSITION_BOTTOM);
  ProtoUtil::defaultFont(timer_text.mutable_font());
  timer_text.mutable_font()->set_size(TIMER_FONT_SIZE);
  renderer->setFont(timer_text.font(), size());
  renderer->setTextColor(Color("white"));
  Position placement = positionText(renderer, timer_text);

  Size shade_size = size();
  shade_size.height = static_cast<int64_t>(
      static_cast<double>(shade_size.height) * TIMER_BACKGROUND_PCT);
  BackgroundImage shading =
      BackgroundImage(shade_size, Color("Black"), TIMER_ALPHA);

  renderer->drawImage(shading, 0,
                      static_cast<int64_t>(static_cast<double>(size().height) *
                                           (1 - TIMER_BACKGROUND_PCT)),
                      true);
  //  auto time =
  //      std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
  std::chrono::duration time =
      std::chrono::system_clock::now().time_since_epoch();
  std::chrono::duration minutes =
      time - std::chrono::duration_cast<std::chrono::hours>(time);
  std::chrono::duration seconds =
      time - std::chrono::duration_cast<std::chrono::minutes>(time);
  std::string curr_time =
      StringUtil::intToString(
          std::chrono::duration_cast<std::chrono::minutes>(minutes).count()) +
      ":" +
      StringUtil::intToString(
          std::chrono::duration_cast<std::chrono::seconds>(seconds).count());
  renderer->drawText(curr_time, placement.x, placement.y);
}

void ScreenTextSide::renderAllText(RenderContext *renderer) {
  for (auto &text : texts) {
    renderText(renderer, &text);
  }
}

auto ScreenTextSide::getTextExtent(RenderContext *renderer,
                                   const std::string &text) -> Size {
  return renderer->textExtent(text);
}

void ScreenTextSide::paintEvent(RenderContext *renderer) {
  renderBackground(renderer);
  renderAllText(renderer);
  renderTimer(renderer);
}

void ScreenTextSide::setImage(const Image &image, bool is_scaled,
                              const proto::ScreenSide &side) {
  if (isSide(side)) {
    image_is_scaled = is_scaled;
    background_color.reset();
    this->image = image;
    background_overlay.reset();
  }
};

void ScreenTextSide::setBackground(const Color &color,
                                   const proto::ScreenSide &side) {
  if (isSide(side)) {
    setBackground(color);
  }
}

void ScreenTextSide::setBackground(const Color &color) {
  this->background_color = color;
  initializeForColor(size(), color);
  background_overlay.reset();
};

void ScreenTextSide::setBackgroundOverlay(const Image &overlay,
                                          double overlay_screen_percentage,
                                          unsigned char overlay_alpha,
                                          OverlayScreenPosition position,
                                          const proto::ScreenSide &side) {
  if (isSide(side)) {
    background_overlay = overlay;
    overlay_percentage = overlay_screen_percentage;
    overlay_position = position;
    this->overlay_alpha = overlay_alpha;
  }
}

void ScreenTextSide::setDefaultBackground(const proto::ScreenSide &side) {
  Color background_color = singleton->teamConfig()->teamColor(side)[0];
  setBackground(background_color, side);
}

void ScreenTextSide::setAll(const ScreenTextSide *source) {
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
  for (const auto &new_text : source->texts) {
    addText(new_text, this->screen_side);
  }

  refresh();
}

auto ScreenTextSide::isSide(const proto::ScreenSide &side) -> bool {
  if (screen_side.home() && side.home()) {
    return true;
  }
  if (screen_side.away() && side.away()) {
    return true;
  }
  return (screen_side.extra() && side.extra());
}

auto ScreenTextSide::ratio(const Size &size) -> float {
  float ratio = 4 / 3;
  ratio = static_cast<float>(size.width) / size.height;
  return ratio;
}

}  // namespace cszb_scoreboard
