/*
ui/component/ScreenText.h: Represents a text presentation on a ScreenPresenter
or a ScreenPreview.

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

#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"
#include "config/Position.h"              // for Size
#include "ui/component/ScreenTextSide.h"  // for ScreenTextSide (ptr only)
#include "ui/widget/Panel.h"              // for Panel
#include "util/Singleton.h"

namespace cszb_scoreboard {
class Color;
class Image;

namespace proto {
class Font;
class RenderableText;
class ScreenSide;
}  // namespace proto
namespace swx {
class Panel;
}  // namespace swx

class ScreenText : public Panel {
 public:
  explicit ScreenText(swx::Panel *wx)
      : ScreenText(wx, Singleton::getInstance()) {}
  void setupPreview(const std::string &initial_text,
                    const std::vector<proto::ScreenSide> &sides, Size size);

  void setupPresenter(const ScreenText &preview, Size size);

  void addText(const proto::RenderableText &text,
               const proto::ScreenSide &side);
  void blackout();
  void resetAllText(const proto::ScreenSide &side);
  void setImage(const Image &image, const proto::ScreenSide &side) {
    setImage(image, true, side);
    refresh();
  }
  void setBackground(const Color &color, const proto::ScreenSide &side);
  void setBackgroundOverlay(const Image &overlay,
                            double overlay_screen_percentage,
                            unsigned char overlay_alpha,
                            OverlayScreenPosition position,
                            const proto::ScreenSide &side);
  void setDefaultBackground(const proto::ScreenSide &side);
  void setFontColor(proto::Font *font, const proto::ScreenSide &side);
  // Resets text fully for screen updates
  void setAllText(const std::string &text, int font_size,
                  const Color &background, bool auto_fit,
                  const proto::ScreenSide &side);
  void setAllText(const std::vector<proto::RenderableText> &lines,
                  const Color &background, bool auto_fit,
                  const proto::ScreenSide &side);
  void setAllText(const std::vector<proto::RenderableText> &lines,
                  const Color &background, bool auto_fit,
                  const Image &logo_overlay, double overlay_screen_percentage,
                  unsigned char logo_alpha, OverlayScreenPosition logo_position,
                  const proto::ScreenSide &side);
  void setAll(const ScreenText &source);
  void setAutoFit(bool auto_fit, const proto::ScreenSide &side);
  auto sides() -> std::vector<ScreenTextSide *> { return text_sides; }

  PUBLIC_TEST_ONLY
  ScreenText(swx::Panel *wx, Singleton *singleton);
#ifdef SCOREBOARD_TESTING
  auto sidePanel(int index) -> ScreenTextSide * { return sides()[index]; }
#endif

 private:
  std::vector<ScreenTextSide *> text_sides;
  bool is_single_view = false;
  Singleton *singleton;

  void autosplitDisplays(const proto::ScreenSide &side);
  void initializeSides(const std::vector<ScreenTextSide *> &text_sides);
  void singleDisplay();
  void setImage(const Image &image, bool is_scaled,
                const proto::ScreenSide &side);
  void setText(const std::string &text, int font_size,
               const proto::ScreenSide &side);
  void splitDisplays();
  static auto splitScreenSize(int x, int y, int number_of_splits) -> Size;
};

}  // namespace cszb_scoreboard
