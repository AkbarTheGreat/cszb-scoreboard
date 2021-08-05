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
  runAgainstActiveContext([image, x, y, use_mask](wxDC* context) -> void {
    context->DrawBitmap(wxBitmap(image), x, y, use_mask);
  });
}

void RenderContext::drawText(const std::string& text, int32_t x, int32_t y) {
  runAgainstActiveContext(
      [text, x, y](wxDC* context) -> void { context->DrawText(text, x, y); });
}

void RenderContext::setFont(const proto::Font& font, const Size& font_size) {
  runAgainstActiveContext([font, font_size](wxDC* context) -> void {
    context->SetFont(ProtoUtil::wxScaledFont(font, font_size.toWx()));
  });
}

void RenderContext::setTextColor(const Color& color) {
  runAgainstActiveContext(
      [color](wxDC* context) -> void { context->SetTextForeground(color); });
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
    runAgainstActiveContext(
        [token, &line_width, &line_height](wxDC* context) -> void {
          context->GetTextExtent(token, &line_width, &line_height);
        });
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

void RenderContext::runAgainstActiveContext(
    const std::function<void(wxDC*)>& lambda) {
  if (event_context) {
    lambda(event_context.get());
  } else if (generic_context) {
    lambda(generic_context.get());
  }
  // If neither of the above is true, this behavior is undefined.
}

}  // namespace cszb_scoreboard
