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

#pragma once

#include <optional>  // for optional
#include <vector>    // for vector

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "config.pb.h"         // for RenderableText, Font (ptr only), Scre...
#include "config/Position.h"
#include "config/swx/image.h"
#include "ui/graphics/Color.h"  // for Color
#include "ui/widget/Panel.h"    // for Panel
#include "ui/widget/RenderContext.h"

class wxPaintEvent;

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

enum class OverlayScreenPosition { Centered, BottomLeft };

class ScreenTextSide : public Panel {
 public:
  ScreenTextSide(swx::Panel *wx, const std::string &initial_text,
                 const proto::ScreenSide &side, Size size);
  ScreenTextSide(swx::Panel *wx, ScreenTextSide *source_side, Size size);

  void addText(const proto::RenderableText &text,
               const proto::ScreenSide &side);
  void blackout();
  void paintEvent(RenderContext *renderer);
  void resetAllText(const proto::ScreenSide &side);
  void setImage(const Image &image, bool is_scaled,
                const proto::ScreenSide &side);
  void setBackground(const Color &color, const proto::ScreenSide &side);
  void setBackgroundOverlay(const Image &overlay,
                            double overlay_screen_percentage,
                            unsigned char overlay_alpha,
                            OverlayScreenPosition position,
                            const proto::ScreenSide &side);
  void setDefaultBackground(const proto::ScreenSide &side);
  void setFontColor(proto::Font *font, const proto::ScreenSide &side);
  void setText(const std::string &text, int font_size,
               const proto::ScreenSide &side);
  void setAll(const ScreenTextSide *source);
  void setAutoFit(bool auto_fit, const proto::ScreenSide &side);
  void setSize(const Size &size);
  auto side() -> const proto::ScreenSide & { return screen_side; }
  auto isSide(const proto::ScreenSide &side) -> bool;

  PUBLIC_TEST_ONLY
  static auto getTextExtent(RenderContext *renderer, const std::string &text) -> Size;

 private:
  bool auto_fit_text;
  Image blackout_image;
  std::optional<Color> background_color;
  std::optional<Image> background_overlay;
  unsigned char overlay_alpha;
  double overlay_percentage;
  OverlayScreenPosition overlay_position;
  Image image;
  bool image_is_scaled;
  proto::ScreenSide screen_side;
  std::vector<proto::RenderableText> texts;

  static auto ratio(const Size &size) -> float;
  static auto scaleImage(const Image &image, const Size &target_size) -> Image;
  void autoFitText(RenderContext *renderer, proto::RenderableText *text);
  void bindEvents();
  void adjustOverlayColorAndAlpha(Image *image, const Color &color) const;
  auto bottomText(RenderContext *renderer, const std::string &text) -> Position;
  auto centerText(RenderContext *renderer, const std::string &text) -> Position;
  void createBlackout();
  void initializeForColor(Size size, Color color);
  auto positionText(RenderContext *renderer, const proto::RenderableText &text) -> Position;
  void renderBackground(RenderContext *renderer);
  void renderOverlay(RenderContext *renderer);
  void renderOverlayBottomCorner(RenderContext *renderer);
  void renderOverlayCentered(RenderContext *renderer);
  void renderScaledBackground(RenderContext *renderer);
  void renderText(RenderContext *dc, proto::RenderableText *text);
  void renderAllText(RenderContext *renderer);
  void setBackground(const Color &color);
  auto topText(RenderContext *renderer, const std::string &text) -> Position;
};

}  // namespace cszb_scoreboard
