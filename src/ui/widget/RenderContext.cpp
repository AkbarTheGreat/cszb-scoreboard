/*
ui/widget/RenderContext.cpp: A class which manages rendering arbitrary data back
out to a widget.

Copyright 2021 Tracy Beck

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

#include "ui/widget/RenderContext.h"

#include <wx/tokenzr.h>  // for wxStringTokenizer, wxTOKEN_...

#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

void RenderContext::drawImage(const Image& image, int32_t x, int32_t y,
                              bool use_mask) {
  if (event_context) {
    event_context->DrawBitmap(wxBitmap(image), x, y, use_mask);
  } else if (generic_context) {
    generic_context->DrawBitmap(wxBitmap(image), x, y, use_mask);
  }
  // If neither of the above is true, this behavior is undefined.
}

void RenderContext::drawText(const std::string& text, int32_t x, int32_t y) {
  if (event_context) {
    event_context->DrawText(text, x, y);
  } else if (generic_context) {
    generic_context->DrawText(text, x, y);
  }
  // If neither of the above is true, this behavior is undefined.
}

// This is a stop-gap implementation of setFont.  Ultimately, we'll move away
// from using wxFont.
void RenderContext::setFont(const proto::Font& font, const Size& font_size) {
  if (event_context) {
    event_context->SetFont(ProtoUtil::wxScaledFont(font, font_size.toWx()));
  } else if (generic_context) {
    generic_context->SetFont(ProtoUtil::wxScaledFont(font, font_size.toWx()));
  }
  // If neither of the above is true, this behavior is undefined.
}

// This is a stop-gap implementation of setFont.  Ultimately, we'll move away
// from using wxFont.
void RenderContext::setTextColor(const Color& color) {
  if (event_context) {
    event_context->SetTextForeground(color);
  } else if (generic_context) {
    generic_context->SetTextForeground(color);
  }
  // If neither of the above is true, this behavior is undefined.
}

void RenderContext::textExtent(const std::string& text, int* width,
                               int* height) {
  if (event_context) {
    event_context->GetTextExtent(text, width, height);
  } else if (generic_context) {
    generic_context->GetTextExtent(text, width, height);
  }
  // If neither of the above is true, this behavior is undefined.
}

auto RenderContext::textExtent(const std::string& text) -> Size {
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
    if (event_context) {
      event_context->GetTextExtent(token, &line_width, &line_height);
    } else if (generic_context) {
      generic_context->GetTextExtent(token, &line_width, &line_height);
    }
    if (line_width > width) {
      width = line_width;
    }
    height += line_height;
  }

  return Size{.width = width, .height = height};
}

auto RenderContext::forEvent(wxWindow* wx) -> std::unique_ptr<RenderContext> {
  auto renderer = std::make_unique<RenderContext>(Token{});
  renderer->event_context = std::make_unique<swx::PaintDC>(wx);
  return renderer;
}

auto RenderContext::forWidget(wxWindow* wx) -> std::unique_ptr<RenderContext> {
  auto renderer = std::make_unique<RenderContext>(Token{});
  renderer->generic_context = std::make_unique<swx::ClientDC>(wx);
  return renderer;
}

}  // namespace cszb_scoreboard
