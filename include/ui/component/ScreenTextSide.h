/*
ui/component/ScreenTextSide.cpp: A single side of renderable image/text.  One or
more of these are contained in a ScreenText, for use in presenting to a preview
or external monitor.

Copyright 2019-2023 Tracy Beck

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
#include <string>    // for string
#include <vector>    // for vector

#include "ScoreboardCommon.h"   // for PUBLIC_TEST_ONLY
#include "config.pb.h"          // for RenderableText, Font (ptr only), Scre...
#include "config/Position.h"    // for Position, Size
#include "config/swx/image.h"   // for Image
#include "ui/graphics/Color.h"  // for Color
#include "ui/widget/Canvas.h"   // for Panel
#include "util/Singleton.h"

namespace cszb_scoreboard {
class RenderContext;

namespace swx {
class Panel;
}  // namespace swx

enum class OverlayScreenPosition { Centered, BottomLeft };

class ScreenTextSide : public Canvas {
 public:
  ScreenTextSide(swx::Panel *wx, const std::string &initial_text,
                 const proto::ScreenSide &side, Size size)
      : ScreenTextSide(wx, initial_text, side, size, Singleton::getInstance()) {
  }
  ScreenTextSide(swx::Panel *wx, ScreenTextSide *source_side, Size size)
      : ScreenTextSide(wx, source_side, size, Singleton::getInstance()) {}

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
  ScreenTextSide(swx::Panel *wx, const std::string &initial_text,
                 const proto::ScreenSide &side, Size size,
                 Singleton *singleton);
  ScreenTextSide(swx::Panel *wx, ScreenTextSide *source_side, Size size,
                 Singleton *singleton);
  static auto getTextExtent(RenderContext *renderer, const std::string &text)
      -> Size;

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
  Singleton *singleton;

  ScreenTextSide(Singleton *singleton, swx::Panel *wx,
                 const proto::ScreenSide &side);
  static auto ratio(const Size &size) -> float;
  static auto scaleImage(const Image &image, const Size &target_size) -> Image;
  void autoFitText(RenderContext *renderer, proto::RenderableText *text);
  void bindEvents();
  void adjustOverlayColorAndAlpha(Image *image, const Color &color) const;
  auto bottomText(RenderContext *renderer, const std::string &text) -> Position;
  auto centerText(RenderContext *renderer, const std::string &text) -> Position;
  void createBlackout();
  void initializeForColor(Size size, const Color &color);
  auto positionText(RenderContext *renderer, const proto::RenderableText &text)
      -> Position;
  void renderBackground(RenderContext *renderer);
  void renderOverlay(RenderContext *renderer);
  void renderOverlayBottomCorner(RenderContext *renderer);
  void renderOverlayCentered(RenderContext *renderer);
  void renderScaledBackground(RenderContext *renderer);
  void renderShadowText(RenderContext *renderer, proto::RenderableText *text);
  void renderText(RenderContext *renderer, proto::RenderableText *text);
  void renderTimer(RenderContext *renderer);
  void renderAllText(RenderContext *renderer);
  void setBackground(const Color &color);
  auto topText(RenderContext *renderer, const std::string &text) -> Position;
};

}  // namespace cszb_scoreboard
