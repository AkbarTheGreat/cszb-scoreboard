/*
ui/widget/RenderContext.h: A class which manages rendering arbitrary data back
out to a widget.

Copyright 2021-2022 Tracy Beck

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

#include <cstdint>     // for int64_t
#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <string>      // for string

#include "config/Position.h"         // for Size
#include "ui/widget/swx/ClientDC.h"  // for ClientDC
#include "ui/widget/swx/PaintDC.h"   // for PaintDC

class wxDC;
class wxWindow;

namespace cszb_scoreboard {
class Color;
class Image;
namespace proto {
class Font;
}  // namespace proto

class RenderContext {
  // Using the passkey idiom from https://abseil.io/tips/134 to avoid adding
  // abseil as a project dependency.  If enough uses of abseil pop up, this can
  // be changed to simply using WrapUnique for singelton construction.
 private:
  class Token {
   private:
    Token() = default;
    friend RenderContext;
  };

 public:
  explicit RenderContext(Token t) {}
  static auto forEvent(wxWindow* wx) -> std::unique_ptr<RenderContext>;
  static auto forWidget(wxWindow* wx) -> std::unique_ptr<RenderContext>;

  void drawImage(const Image& image, int64_t x, int64_t y,
                 bool use_mask = false);
  void drawText(const std::string& text, int64_t x, int64_t y);
  void setFont(const proto::Font& font, const Size& font_size);
  void setTextColor(const Color& color);
  void textExtent(const std::string& text, int* width, int* height);
  auto textExtent(const std::string& text) -> Size;

 private:
  // Any initialization of either of these objects should ensure that the other
  // is empty, as this object is only intended to represent one of these
  // contexts at a time.
  std::unique_ptr<swx::ClientDC> generic_context;
  std::unique_ptr<swx::PaintDC> event_context;

  void runAgainstActiveContext(const std::function<void(wxDC*)>& lambda);
};

}  // namespace cszb_scoreboard
