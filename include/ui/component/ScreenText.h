/*
ui/component/ScreenText.h: Represents a text presentation on a ScreenPresenter
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

#pragma once

#include <wx/wx.h>

#include <optional>
#include <vector>

#include "config.pb.h"
#include "ui/component/ScreenTextSide.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

class ScreenText : public wxPanel {
 public:
  // These two methods are really just light wrappers around constructors, but
  // they help document the use each constructor is intended to fill.
  static auto getPreview(wxWindow* parent, const wxString& initial_text,
                         const std::vector<proto::ScreenSide>& sides,
                         wxSize size) -> ScreenText*;

  static auto getPresenter(wxWindow* parent, ScreenText* preview, wxSize size)
      -> ScreenText*;

  void addText(const proto::RenderableText& text,
               const proto::ScreenSide& side);
  void blackout();
  void resetAllText(const proto::ScreenSide& side);
  void setImage(const wxImage& image, const proto::ScreenSide& side) {
    setImage(image, true, side);
    Refresh();
  }
  void setBackground(const Color& color, const proto::ScreenSide& side);
  void setBackgroundOverlay(const wxImage& overlay,
                            double overlay_screen_percentage,
                            unsigned char overlay_alpha,
                            OverlayScreenPosition position,
                            const proto::ScreenSide& side);
  void setDefaultBackground(const proto::ScreenSide& side);
  void setFontColor(proto::Font* font, const proto::ScreenSide& side);
  // Resets text fully for screen updates
  void setAllText(const wxString& text, int font_size, const Color& background,
                  bool auto_fit, const proto::ScreenSide& side);
  void setAllText(const std::vector<proto::RenderableText>& lines,
                  const Color& background, bool auto_fit,
                  const proto::ScreenSide& side);
  void setAllText(const std::vector<proto::RenderableText>& lines,
                  const Color& background, bool auto_fit,
                  const wxImage& logo_overlay, double overlay_screen_percentage,
                  unsigned char logo_alpha, OverlayScreenPosition logo_position,
                  const proto::ScreenSide& side);
  void setAll(const ScreenText& source);
  void setAutoFit(bool auto_fit, const proto::ScreenSide& side);
  auto sides() -> std::vector<ScreenTextSide*> { return text_sides; }

#ifdef SCOREBOARD_TESTING
  auto sidePanel(int index) -> wxPanel* { return sides()[index]; }
#endif

 private:
  std::vector<ScreenTextSide*> text_sides;
  bool is_single_view = false;

  ScreenText(wxWindow* parent, wxSize size)
      : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxTAB_TRAVERSAL) {}

  void autosplitDisplays(const proto::ScreenSide& side);
  void initializeSides(const std::vector<ScreenTextSide*>& text_sides);
  void singleDisplay();
  void setImage(const wxImage& image, bool is_scaled,
                const proto::ScreenSide& side);
  void setText(const wxString& text, int font_size,
               const proto::ScreenSide& side);
  void splitDisplays();
  static auto splitScreenSize(int x, int y, int number_of_splits) -> wxSize;
};

}  // namespace cszb_scoreboard
