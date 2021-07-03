/*
ui/component/ScreenText.cpp: Represents a text presentation on a ScreenPresenter
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

#include "ui/component/ScreenText.h"

#include <wx/image.h>
#include <wx/tokenzr.h>

#include "config/TeamConfig.h"
#include "ui/UiUtil.h"
#include "ui/graphics/BackgroundImage.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

// Margin for the top or bottom, as a percentage
const float TOP_OR_BOTTOM_MARGIN = 2;
const int BORDER_SIZE = 0;

void ScreenText::setupPreview(const wxString& initial_text,
                              const std::vector<proto::ScreenSide>& sides,
                              wxSize size) {
  SetSize(size);
  std::vector<ScreenTextSide*> text_sides;

  wxSize split_size = splitScreenSize(size.x, size.y, sides.size());

  std::vector<proto::TeamInfo_TeamType> screen_order =
      TeamConfig::getInstance()->singleScreenOrder();
  // Add the error value to the back, so it can still be created if needed
  screen_order.push_back(proto::TeamInfo_TeamType_TEAM_ERROR);

  for (auto team : screen_order) {
    for (const auto& side : sides) {
      if (ProtoUtil::sideContains(side, team)) {
        text_sides.push_back(
            new ScreenTextSide(wx, initial_text, side, split_size));
        break;
      }
    }
  }

  initializeSides(text_sides);
}

void ScreenText::setupPresenter(const ScreenText& preview, wxSize size) {
  SetSize(size);
  std::vector<ScreenTextSide*> text_sides;

  wxSize split_size =
      splitScreenSize(size.x, size.y, preview.text_sides.size());

  for (auto* source_text_side : preview.text_sides) {
    text_sides.push_back(new ScreenTextSide(wx, source_text_side, split_size));
  }

  initializeSides(text_sides);
}

void ScreenText::initializeSides(
    const std::vector<ScreenTextSide*>& text_sides) {
  for (auto* text_side : this->text_sides) {
    delete text_side;
  }
  this->text_sides.clear();

  wxSizer* sizer = UiUtil::sizer(1, 0);
  for (auto* text_side : text_sides) {
    this->text_sides.push_back(text_side);
    sizer->Add(text_side, 0, wxALL, BORDER_SIZE);
  }
  SetSizerAndFit(sizer);
}

void ScreenText::resetAllText(const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->resetAllText(side);
  }
}

void ScreenText::setAutoFit(bool auto_fit, const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->setAutoFit(auto_fit, side);
  }
}

void ScreenText::addText(const proto::RenderableText& text,
                         const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->addText(text, side);
  }
}

void ScreenText::setText(const wxString& text, int font_size,
                         const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->setText(text, font_size, side);
  }
}

void ScreenText::setAllText(const wxString& text, int font_size,
                            const Color& background, bool auto_fit,
                            const proto::ScreenSide& side) {
  setAutoFit(auto_fit, side);
  setBackground(background, side);
  setText(text, font_size, side);
  Refresh();
}

void ScreenText::setAllText(const std::vector<proto::RenderableText>& lines,
                            const Color& background, bool auto_fit,
                            const proto::ScreenSide& side) {
  setAutoFit(auto_fit, side);
  resetAllText(side);
  setBackground(background, side);
  for (auto line : lines) {
    ProtoUtil::validateFont(line.mutable_font());
    setFontColor(line.mutable_font(), side);
    addText(line, side);
    Refresh();
  }
}

void ScreenText::setAllText(const std::vector<proto::RenderableText>& lines,
                            const Color& background, bool auto_fit,
                            const wxImage& logo_overlay,
                            double overlay_screen_percentage,
                            unsigned char logo_alpha,
                            OverlayScreenPosition logo_position,
                            const proto::ScreenSide& side) {
  setAutoFit(auto_fit, side);
  resetAllText(side);
  setBackground(background, side);
  setBackgroundOverlay(logo_overlay, overlay_screen_percentage, logo_alpha,
                       logo_position, side);
  for (auto line : lines) {
    ProtoUtil::validateFont(line.mutable_font());
    setFontColor(line.mutable_font(), side);
    addText(line, side);
    Refresh();
  }
}

void ScreenText::setFontColor(proto::Font* font,
                              const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto& text_side : text_sides) {
    text_side->setFontColor(font, side);
  }
}

void ScreenText::blackout() {
  singleDisplay();
  text_sides[0]->blackout();
}

void ScreenText::setImage(const wxImage& image, bool is_scaled,
                          const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->setImage(image, is_scaled, side);
  }
}

void ScreenText::setBackground(const Color& color,
                               const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->setBackground(color, side);
  }
}

void ScreenText::setBackgroundOverlay(const wxImage& overlay,
                                      double overlay_screen_percentage,
                                      unsigned char overlay_alpha,
                                      OverlayScreenPosition position,
                                      const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->setBackgroundOverlay(overlay, overlay_screen_percentage,
                                    overlay_alpha, position, side);
  }
}

void ScreenText::setDefaultBackground(const proto::ScreenSide& side) {
  autosplitDisplays(side);
  for (auto* text_side : text_sides) {
    text_side->setDefaultBackground(side);
  }
}

void ScreenText::autosplitDisplays(const proto::ScreenSide& side) {
  for (auto* text_side : text_sides) {
    if (!text_side->isSide(side)) {
      splitDisplays();
      return;
    }
  }
  singleDisplay();
}

void ScreenText::singleDisplay() {
  is_single_view = true;
  if (text_sides.size() < 2) {
    return;
  }
  text_sides[0]->setSize(GetSize());
  for (int i = 1; i < text_sides.size(); i++) {
    text_sides[i]->Hide();
  }
  Update();
}

void ScreenText::splitDisplays() {
  is_single_view = false;
  if (text_sides.size() < 2) {
    return;
  }
  text_sides[0]->setSize(
      splitScreenSize(GetSize().x, GetSize().y, text_sides.size()));
  for (int i = 1; i < text_sides.size(); i++) {
    text_sides[i]->Show();
  }
  Update();
}

auto ScreenText::splitScreenSize(int x, int y, int number_of_splits) -> wxSize {
  if (number_of_splits > 0) {
    x = x / number_of_splits;
  }
  return wxSize(x, y);
}

void ScreenText::setAll(const ScreenText& source) {
  if (source.is_single_view) {
    singleDisplay();
  } else {
    splitDisplays();
  }

  for (auto* target_side : text_sides) {
    for (auto* source_side : source.text_sides) {
      if (target_side->isSide(source_side->side())) {
        target_side->setAll(source_side);
      }
    }
  }
}

}  // namespace cszb_scoreboard
