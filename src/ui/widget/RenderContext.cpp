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

namespace cszb_scoreboard {

void RenderContext::drawImage(const Image& image, int32_t x, int32_t y,
                              bool use_mask) {
  if (event_context) {
    event_context->DrawBitmap(wxBitmap(image), x, y, false);
  } else if (generic_context) {
    event_context->DrawBitmap(wxBitmap(image), x, y, false);
  }
  // If neither of the above is true, this behavior is undefined.
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
