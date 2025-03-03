/*
ui/component/ScreenText.cpp: Represents a text presentation on a ScreenPresenter
or a ScreenPreview.

Copyright 2019-2025 Tracy Beck

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

#include <wx/event.h>                     // for wxEVT_PAINT
#include <algorithm>                      // for max

#include "config.pb.h"                    // for RenderableText, ScreenSide
#include "config/SlideShow.h"             // for SlideShow
#include "config/TeamConfig.h"            // for TeamConfig
#include "ui/component/ScreenTextSide.h"  // for ScreenTextSide, ScreenTextC...
#include "util/ProtoUtil.h"               // for ProtoUtil

namespace cszb_scoreboard {
class Color;
class Image;
class RenderContext;

// Margin for the top or bottom, as a percentage
const float TOP_OR_BOTTOM_MARGIN = 2;
const int BORDER_SIZE = 0;

ScreenText::ScreenText(swx::Panel *wx, Singleton *singleton) : Panel(wx) {
  this->singleton = singleton;
  bindEvents();
}

void ScreenText::bindEvents() {
  bind(wxEVT_PAINT,
       [this](RenderContext *renderer) -> void { this->paintEvent(renderer); });
}

void ScreenText::paintEvent(RenderContext *renderer) {
  if (category == ScreenTextCategory::Preview) {
    return;
  }

  if (singleton->slideShow()->isRunning()) {
    singleDisplay(/*is_temporary=*/true);
    return;
  }

  if (view_split_override) {
    if (is_single_view) {
      singleDisplay();
    } else {
      splitDisplays();
    }
    return;
  }
}

void ScreenText::setupPreview(const std::string &initial_text,
                              const std::vector<proto::ScreenSide> &sides,
                              Size size) {
  category = ScreenTextCategory::Preview;
  setSize(size);
  std::vector<ScreenTextSide *> text_sides;

  Size split_size = splitScreenSize(size.width, size.height, sides.size());

  std::vector<proto::TeamInfo_TeamType> screen_order =
      singleton->teamConfig()->singleScreenOrder();
  // Add the error value to the back, so it can still be created if needed
  screen_order.push_back(proto::TeamInfo_TeamType_TEAM_ERROR);

  for (auto team : screen_order) {
    for (const auto &side : sides) {
      if (ProtoUtil::sideContains(side, team)) {
        text_sides.push_back(
            new ScreenTextSide(childPanel(split_size.width, split_size.height),
                               initial_text, side, split_size, category));
        break;
      }
    }
  }

  initializeSides(text_sides);
}

void ScreenText::setupPresenter(const ScreenText &preview, Size size) {
  category = ScreenTextCategory::Presenter;
  setSize(size);
  std::vector<ScreenTextSide *> text_sides;
  text_sides.reserve(preview.text_sides.size());

  Size split_size =
      splitScreenSize(size.width, size.height, preview.text_sides.size());

  for (auto *source_text_side : preview.text_sides) {
    text_sides.push_back(
        new ScreenTextSide(childPanel(split_size.width, split_size.height),
                           source_text_side, split_size, category));
  }

  initializeSides(text_sides);
}

void ScreenText::initializeSides(
    const std::vector<ScreenTextSide *> &text_sides) {
  for (auto *text_side : this->text_sides) {
    delete text_side;
  }
  this->text_sides.clear();
  this->text_sides.reserve(text_sides.size());

  int col = 0;
  for (auto *text_side : text_sides) {
    this->text_sides.emplace_back(text_side);
    addWidget(*text_side, 0, col++, BORDER_SIZE);
  }
  runSizer();
}

void ScreenText::resetAllText(const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->resetAllText(side);
  }
}

void ScreenText::setAutoFit(bool auto_fit, const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->setAutoFit(auto_fit, side);
  }
}

void ScreenText::addText(const proto::RenderableText &text,
                         const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->addText(text, side);
  }
}

void ScreenText::setText(const std::string &text, int font_size,
                         const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->setText(text, font_size, side);
  }
}

void ScreenText::setAllText(const std::string &text, int font_size,
                            const Color &background, bool auto_fit,
                            const proto::ScreenSide &side) {
  setAutoFit(auto_fit, side);
  setBackground(background, side);
  setText(text, font_size, side);
  refresh();
}

void ScreenText::setAllText(const std::vector<proto::RenderableText> &lines,
                            const Color &background, bool auto_fit,
                            const proto::ScreenSide &side) {
  setAutoFit(auto_fit, side);
  resetAllText(side);
  setBackground(background, side);
  for (auto line : lines) {
    ProtoUtil::validateFont(line.mutable_font());
    setFontColor(line.mutable_font(), side);
    addText(line, side);
    refresh();
  }
}

void ScreenText::setAllText(const std::vector<proto::RenderableText> &lines,
                            const Color &background, bool auto_fit,
                            const Image &logo_overlay,
                            double overlay_screen_percentage,
                            unsigned char logo_alpha,
                            OverlayScreenPosition logo_position,
                            const proto::ScreenSide &side) {
  setAutoFit(auto_fit, side);
  resetAllText(side);
  setBackground(background, side);
  setBackgroundOverlay(logo_overlay, overlay_screen_percentage, logo_alpha,
                       logo_position, side);
  for (auto line : lines) {
    ProtoUtil::validateFont(line.mutable_font());
    setFontColor(line.mutable_font(), side);
    addText(line, side);
    refresh();
  }
}

void ScreenText::setFontColor(proto::Font *font,
                              const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto &text_side : text_sides) {
    text_side->setFontColor(font, side);
  }
}

void ScreenText::blackout() {
  singleDisplay();
  text_sides[0]->blackout();
}

void ScreenText::setImage(const Image &image, bool is_scaled,
                          const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->setImage(image, is_scaled, side);
  }
}

void ScreenText::setBackground(const Color &color,
                               const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->setBackground(color, side);
  }
}

void ScreenText::setBackgroundOverlay(const Image &overlay,
                                      double overlay_screen_percentage,
                                      unsigned char overlay_alpha,
                                      OverlayScreenPosition position,
                                      const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->setBackgroundOverlay(overlay, overlay_screen_percentage,
                                    overlay_alpha, position, side);
  }
}

void ScreenText::setDefaultBackground(const proto::ScreenSide &side) {
  autosplitDisplays(side);
  for (auto *text_side : text_sides) {
    text_side->setDefaultBackground(side);
  }
}

void ScreenText::autosplitDisplays(const proto::ScreenSide &side) {
  for (auto *text_side : text_sides) {
    if (!text_side->isSide(side)) {
      splitDisplays();
      return;
    }
  }
  singleDisplay();
}

void ScreenText::singleDisplay(bool is_temporary) {
  if (!is_temporary) {
    is_single_view = true;
  }
  view_split_override = is_temporary;
  if (text_sides.size() < 2) {
    return;
  }
  text_sides[0]->setSize(size());
  for (int i = 1; i < text_sides.size(); i++) {
    text_sides[i]->hide();
  }
  update();
}

void ScreenText::splitDisplays(bool is_temporary) {
  if (!is_temporary) {
    is_single_view = false;
  }
  view_split_override = is_temporary;
  if (text_sides.size() < 2) {
    return;
  }
  text_sides[0]->setSize(
      splitScreenSize(size().width, size().height, text_sides.size()));
  for (int i = 1; i < text_sides.size(); i++) {
    text_sides[i]->show();
  }
  update();
}

auto ScreenText::splitScreenSize(int x, int y, int number_of_splits) -> Size {
  if (number_of_splits > 0) {
    x = x / number_of_splits;
  }
  return Size{.width = x, .height = y};
}

void ScreenText::setAll(const ScreenText &source) {
  if (source.is_single_view) {
    singleDisplay();
  } else {
    splitDisplays();
  }

  for (auto *target_side : text_sides) {
    for (auto *source_side : source.text_sides) {
      if (target_side->isSide(source_side->side())) {
        target_side->setAll(source_side);
      }
    }
  }
}

}  // namespace cszb_scoreboard
