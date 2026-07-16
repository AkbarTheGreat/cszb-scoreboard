/*
ui/widget/RenderContext.cpp: A class which manages rendering arbitrary data back
out to a widget.

Copyright 2021-2026 Tracy Beck

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
#include <wx/font.h>     // for wxFont, wxFontFamily, wxFontStyle
#include <wx/string.h>   // for wxString
#include <wx/tokenzr.h>  // for wxStringTokenizer, wxStringTokenizerMode

#include "config.pb.h"          // for Font
#include "ui/graphics/Color.h"  // for Color
#include "ui/widget/Image.h"    // for Image
#include "util/ProtoUtil.h"     // for ProtoUtil
#include "wx/bitmap.h"          // for wxBitmap
#include "wx/brush.h"           // IWYU pragma: keep for wxBrush
#include "wx/colour.h"          // for wxColor, wxColour
#include "wx/gdicmn.h"          // for wxTRANSPARENT_PEN, wxWHITE
#include "wx/pen.h"             // IWYU pragma: keep for wxPen

// IWYU pragma: no_include <bits/chrono.h>
// IWYU pragma: no_include "wx/gtk/brush.h"
// IWYU pragma: no_include "wx/gtk/pen.h"

class wxWindow;

namespace cszb_scoreboard {

/*
Draws a small "GIF" stamp at the bottom-right corner of an image to indicate
that it is an animated GIF.  This is used when the image is rendered as a static
image.
 */
void drawGifStamp(wxDC* context, const Position& image_position,
                  const Size& image_size) {
  // Draw a small "GIF" stamp at the bottom-right corner of the image
  constexpr int STAMP_WIDTH = 28;
  constexpr int STAMP_HEIGHT = 14;
  constexpr int STAMP_PADDING = 4;
  constexpr int STAMP_BG_COLOR_VAL = 50;
  constexpr int STAMP_BG_ALPHA = 180;
  constexpr int STAMP_FONT_SIZE = 8;
  constexpr int STAMP_TEXT_X_OFFSET = 3;

  int stamp_x =
      image_position.x + image_size.width - STAMP_WIDTH - STAMP_PADDING;
  int stamp_y =
      image_position.y + image_size.height - STAMP_HEIGHT - STAMP_PADDING;
  if (stamp_x >= image_position.x && stamp_y >= image_position.y) {
    wxPen oldPen = context->GetPen();
    wxBrush oldBrush = context->GetBrush();
    wxFont oldFont = context->GetFont();
    wxColor oldTextForeground = context->GetTextForeground();

    context->SetPen(*wxTRANSPARENT_PEN);
    // Dark grey translucent brush
    context->SetBrush(wxBrush(wxColor(STAMP_BG_COLOR_VAL, STAMP_BG_COLOR_VAL,
                                      STAMP_BG_COLOR_VAL, STAMP_BG_ALPHA)));
    context->DrawRectangle(stamp_x, stamp_y, STAMP_WIDTH, STAMP_HEIGHT);

    context->SetTextForeground(*wxWHITE);
    wxFont font(STAMP_FONT_SIZE, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                wxFONTWEIGHT_BOLD);
    context->SetFont(font);
    context->DrawText("GIF", stamp_x + STAMP_TEXT_X_OFFSET, stamp_y);

    context->SetPen(oldPen);
    context->SetBrush(oldBrush);
    context->SetFont(oldFont);
    context->SetTextForeground(oldTextForeground);
  }
}

void RenderContext::clear(const Color& color) {
  runAgainstActiveContext([color](wxDC* context) -> void {
    context->SetBackground(wxBrush(color));
    context->Clear();
  });
}

void RenderContext::drawImage(const Image& image, int64_t x, int64_t y,
                              bool use_mask, bool animate) {
  runAgainstActiveContext(
      [image, x, y, use_mask, animate](wxDC* context) -> void {
        if (image.isAnimated() && animate) {
          auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::steady_clock::now().time_since_epoch())
                         .count();
          context->DrawBitmap(wxBitmap(image.animate(now)), x, y, use_mask);
        } else {
          context->DrawBitmap(wxBitmap(image.wx()), x, y, use_mask);
          if (image.isAnimated() && !animate) {
            drawGifStamp(context, Position{x, y}, image.size());
          }
        }
      });
}

void RenderContext::drawLine(const Position& start, const Position& end) {
  runAgainstActiveContext([start, end](wxDC* context) -> void {
    context->DrawLine(start.toWx(), end.toWx());
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

void RenderContext::setPen(const Color& color, int width) {
  runAgainstActiveContext([color, width](wxDC* context) -> void {
    context->SetPen(wxPen(color, width));
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

auto RenderContext::forDC(wxDC* dc) -> std::unique_ptr<RenderContext> {
  auto renderer = std::make_unique<RenderContext>(Token{});
  renderer->raw_context = dc;
  return renderer;
}

void RenderContext::runAgainstActiveContext(
    const std::function<void(wxDC*)>& lambda) {
  if (raw_context != nullptr) {
    lambda(raw_context);
  } else if (event_context != nullptr) {
    lambda(event_context.get());
  } else if (generic_context != nullptr) {
    lambda(generic_context.get());
  }
  // If neither of the above is true, this behavior is undefined.
}

}  // namespace cszb_scoreboard
