/*
ui/widget/RenderContext.cpp: A class which manages rendering arbitrary data back
out to a widget.

Copyright 2021-2023 Tracy Beck

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

#include <wx/dc.h>       // for wxDC
#include <wx/string.h>   // for wxString
#include <wx/tokenzr.h>  // for wxStringTokenizer, wxTOKEN_RET_EMPTY_ALL

#include "config.pb.h"          // for Font
#include "config/swx/image.h"   // for Image
#include "ui/graphics/Color.h"  // for Color
#include "util/ProtoUtil.h"     // for ProtoUtil
#include "wx/bitmap.h"          // for wxBitmap

class wxWindow;

namespace cszb_scoreboard {

void RenderContext::drawImage(const Image& image, int64_t x, int64_t y,
                              bool use_mask) {
  runAgainstActiveContext([image, x, y, use_mask](wxDC* context) -> void {
    context->DrawBitmap(wxBitmap(image), x, y, use_mask);
  });
}

void RenderContext::drawText(const std::string& text, int64_t x, int64_t y) {
  runAgainstActiveContext(
      [text, x, y](wxDC* context) -> void { context->DrawText(text, x, y); });
}

void RenderContext::setFont(const proto::Font& font, const Size& font_size) {
  runAgainstActiveContext([font, font_size](wxDC* context) -> void {
    context->SetFont(ProtoUtil::wxScaledFont(font, font_size));
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

/* Gets a render context for a widget during a paint event.  This is the most
 * common use case for a RenderContext, by far.
 */
auto RenderContext::forEvent(wxWindow* wx) -> std::unique_ptr<RenderContext> {
  auto renderer = std::make_unique<RenderContext>(Token{});
  renderer->event_context = std::make_unique<swx::PaintDC>(wx);
  return renderer;
}

/* Gets a render context for a widget outside of an event, for a one-off
 * rendering.  Note that this is not buffered and so may add flicker when
 * called.  Currently, this is test-only, so that's fine, but this may need an
 * update if used in the application-proper.  (Note to future developers:  Doing
 * so will require adding a destructor and ensuring that the buffered client DC
 * is destroyed before the client DC).
 */
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
