/*
ui/component/ScreenTextSide.cpp: A single side of renderable image/text.  One or
more of these are contained in a ScreenText, for use in presenting to a preview
or external monitor.

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

#pragma once

#include <wx/wx.h>

#include <optional>
#include <vector>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

enum class OverlayScreenPosition { Centered, BottomLeft };

class ScreenTextSide : public wxPanel {
 public:
  ScreenTextSide(wxWindow* parent, const wxString& initial_text,
                 proto::ScreenSide side, wxSize size);
  ScreenTextSide(wxWindow* parent, ScreenTextSide* source_side, wxSize size);

  void addText(proto::RenderableText text, const proto::ScreenSide& side);
  void blackout();
  void paintEvent(wxPaintEvent& event);
  void resetAllText(const proto::ScreenSide& side);
  void setImage(const wxImage& image, bool is_scaled,
                const proto::ScreenSide& side);
  void setBackground(const Color& color, const proto::ScreenSide& side);
  void setBackgroundOverlay(const wxImage& overlay,
                            double overlay_screen_percentage,
                            unsigned char overlay_alpha,
                            OverlayScreenPosition position,
                            const proto::ScreenSide& side);
  void setDefaultBackground(const proto::ScreenSide& side);
  void setFontColor(proto::Font* font, const proto::ScreenSide& side);
  void setText(const wxString& text, int font_size,
               const proto::ScreenSide& side);
  void setAll(const ScreenTextSide* source);
  void setAutoFit(bool auto_fit, const proto::ScreenSide& side);
  void setSize(const wxSize& size);
  const proto::ScreenSide& side() { return screen_side; }
  bool isSide(proto::ScreenSide side);

  PUBLIC_TEST_ONLY
  wxSize getTextExtent(wxDC& dc, wxString text);

 private:
  bool auto_fit_text;
  wxImage blackout_image;
  std::optional<Color> background_color;
  std::optional<wxImage> background_overlay;
  unsigned char overlay_alpha;
  double overlay_percentage;
  OverlayScreenPosition overlay_position;
  wxImage image;
  bool image_is_scaled;
  proto::ScreenSide screen_side;
  std::vector<proto::RenderableText> texts;

  void autoFitText(wxDC& dc, proto::RenderableText& text);
  void bindEvents();
  void adjustOverlayColorAndAlpha(wxImage& image, const Color& color);
  wxPoint bottomText(wxDC& dc, wxString text);
  wxPoint centerText(wxDC& dc, wxString text);
  void createBlackout();
  void initializeForColor(wxSize size, Color color);
  wxPoint positionText(wxDC& dc, proto::RenderableText text);
  float ratio(const wxSize& size);
  void renderBackground(wxDC& dc);
  void renderOverlay(wxDC& dc);
  void renderOverlayBottomCorner(wxDC& dc);
  void renderOverlayCentered(wxDC& dc);
  void renderScaledBackground(wxDC& dc);
  void renderText(wxDC& dc, proto::RenderableText& text);
  void renderAllText(wxDC& dc);
  wxImage scaleImage(const wxImage& image, const wxSize& target_size);
  void setBackground(const Color& color);
  wxPoint topText(wxDC& dc, wxString text);
};

}  // namespace cszb_scoreboard
