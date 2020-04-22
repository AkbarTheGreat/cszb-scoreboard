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
#include "ui/UiUtil.h"
#include "ui/graphics/BackgroundImage.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

// Margin for the top or bottom, as a percentage
const float TOP_OR_BOTTOM_MARGIN = 2;
const int BORDER_SIZE = 0;

ScreenText* ScreenText::getPreview(wxWindow* parent, wxString initial_text,
                                   std::vector<proto::ScreenSide> sides,
                                   wxSize size) {
  ScreenText* screen_text = new ScreenText(parent, size);
  std::vector<ScreenTextSide*> text_sides;

  wxSize split_size(size.x / sides.size(), size.y);

  for (auto side : sides) {
    text_sides.push_back(
        new ScreenTextSide(screen_text, initial_text, side, split_size));
  }

  screen_text->initializeSides(text_sides);
  return screen_text;
}

ScreenText* ScreenText::getPresenter(wxWindow* parent, ScreenText* preview,
                                     wxSize size) {
  ScreenText* screen_text = new ScreenText(parent, size);
  std::vector<ScreenTextSide*> text_sides;

  wxSize split_size(size.x / preview->text_sides.size(), size.y);

  for (auto source_text_side : preview->text_sides) {
    text_sides.push_back(
        new ScreenTextSide(screen_text, source_text_side, split_size));
  }

  screen_text->initializeSides(text_sides);
  return screen_text;
}

void ScreenText::initializeSides(std::vector<ScreenTextSide*> text_sides) {
  for (auto text_side : this->text_sides) {
    delete text_side;
  }
  this->text_sides.clear();

  for (auto text_side : text_sides) {
    this->text_sides.push_back(text_side);
  }
  wxSizer* sizer = UiUtil::sizer(1, 0);
  for (auto text_side : text_sides) {
    sizer->Add(text_side, 0, wxALL, BORDER_SIZE);
  }
  SetSizerAndFit(sizer);
}

void ScreenText::resetAllText(const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->resetAllText(side);
  }
}

void ScreenText::setAutoFit(bool auto_fit, const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->setAutoFit(auto_fit, side);
  }
}

void ScreenText::addText(proto::RenderableText text,
                         const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->addText(text, side);
  }
}

void ScreenText::setText(const wxString& text, int font_size,
                         const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->setText(text, font_size, side);
  }
}

void ScreenText::setFontColor(proto::Font* font) {
  for (auto text_side : text_sides) {
    text_side->setFontColor(font);
  }
}

void ScreenText::blackout() {
  for (auto text_side : text_sides) {
    text_side->blackout();
  }
}

void ScreenText::setImage(const wxImage& image) {
  for (auto text_side : text_sides) {
    text_side->setImage(image);
  }
}

void ScreenText::setImage(const wxImage& image, bool is_scaled,
                          const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->setImage(image, is_scaled, side);
  }
}

void ScreenText::setBackground(const Color& color,
                               const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->setBackground(color, side);
  }
}

void ScreenText::setDefaultBackground(const proto::ScreenSide& side) {
  for (auto text_side : text_sides) {
    text_side->setDefaultBackground(side);
  }
}

void ScreenText::setAll(const ScreenText& source) {
  // TODO: This assumes both have the same number of sides, and may not be
  // useful long-term
  for (int i = 0; i < text_sides.size(); i++) {
    this->text_sides[i]->setAll(source.text_sides[i]);
  }
}

bool ScreenText::isSide(proto::ScreenSide side) {
  for (auto text_side : text_sides) {
    if (text_side->isSide(side)) {
      return true;
    }
  }
  return false;
}

}  // namespace cszb_scoreboard
