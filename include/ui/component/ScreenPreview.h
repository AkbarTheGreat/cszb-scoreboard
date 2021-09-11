/*
ui/component/ScreenPreview.h: This class manages the thumbnail preview of a
monitor in the main window.  In addition, the preview owns the ScreenPresenter
which displays to the actual monitor, dispatching the updated view when
necessary.

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

#include <memory>  // for unique_ptr
#include <string>  // for string
#include <vector>  // for vector

#include "ScoreboardCommon.h"
#include "config/Position.h"               // for Size
#include "ui/component/ScreenText.h"       // for ScreenText
#include "ui/component/ScreenTextSide.h"   // for OverlayScreenPosition
#include "ui/component/ScreenThumbnail.h"  // for ScreenThumbnail
#include "ui/widget/Panel.h"               // for Panel
#include "util/Singleton.h"

namespace cszb_scoreboard {
class Color;
class ScreenPresenter;
class Image;

namespace proto {
class RenderableText;
class ScreenSide;
}  // namespace proto
namespace swx {
class Panel;
}  // namespace swx

class ScreenPreview : public Panel {
 public:
  ScreenPreview(swx::Panel *wx, std::vector<proto::ScreenSide> sides,
                int monitor_number)
      : ScreenPreview(wx, sides, monitor_number, Singleton::getInstance()) {}
  void sendToPresenter(ScreenText *screen_text);
  void sendToPresenter();
  void blackoutPresenter();
  void resetFromSettings(int monitor_number);
  auto screen() -> ScreenText * { return screen_text.get(); }
  auto thumbnailWidget() -> ScreenText *;

  void setAllText(const std::string &text, int font_size,
                  const Color &background, bool auto_fit,
                  const proto::ScreenSide &side) {
    screen_text->setAllText(text, font_size, background, auto_fit, side);
  }
  void setAllText(const std::vector<proto::RenderableText> &lines,
                  const Color &background, bool auto_fit,
                  const proto::ScreenSide &side) {
    screen_text->setAllText(lines, background, auto_fit, side);
  }
  void setAllText(const std::vector<proto::RenderableText> &lines,
                  const Color &background, bool auto_fit,
                  const Image &logo_overlay, double overlay_screen_percentage,
                  unsigned char logo_alpha, OverlayScreenPosition logo_position,
                  const proto::ScreenSide &side) {
    screen_text->setAllText(lines, background, auto_fit, logo_overlay,
                            overlay_screen_percentage, logo_alpha,
                            logo_position, side);
  }
  void setAll(const ScreenText &source) { screen_text->setAll(source); }
  void setImage(const Image &image, const proto::ScreenSide &side) {
    screen_text->setImage(image, side);
  }

  PUBLIC_TEST_ONLY
  ScreenPreview(swx::Panel *wx, std::vector<proto::ScreenSide> sides,
                int monitor_number, Singleton *singleton);

 private:
  std::unique_ptr<ScreenText> screen_text;
  ScreenPresenter *presenter;
  std::unique_ptr<ScreenThumbnail> thumbnail;
  Singleton *singleton;
  auto previewSize(int monitor_number) -> Size;
  void positionWidgets();
};
}  // namespace cszb_scoreboard
