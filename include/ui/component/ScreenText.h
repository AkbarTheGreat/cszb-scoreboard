/*
ui/component/ScreenText.h: Represents a text presentation on a ScreenPresenter
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

#pragma once

#include <wx/wx.h>

#include <optional>
#include <vector>

#include "config.pb.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

class ScreenText : public wxPanel {
 public:
  // These two methods are really just light wrappers around constructors, but
  // they help document the use each constructor is intended to fill.
  static ScreenText* getPreview(wxWindow* parent, wxString initial_text,
                                proto::ScreenSide side, wxSize size);

  static ScreenText* getPresenter(wxWindow* parent, ScreenText* preview,
                                  wxSize size);

  void addText(proto::RenderableText text, const proto::ScreenSide& side);
  void blackout();
  void paintEvent(wxPaintEvent& event);
  void resetAllText(const proto::ScreenSide& side);
  void setImage(const wxImage& image);
  void setImage(const wxImage& image, bool is_scaled,
                const proto::ScreenSide& side);
  void setBackground(const Color& color, const proto::ScreenSide& side);
  void setDefaultBackground(const proto::ScreenSide& side);
  void setFontColor(proto::Font* font);
  void setText(const wxString& text, int font_size,
               const proto::ScreenSide& side);
  void setAll(const ScreenText& source);
  bool isSide(proto::ScreenSide side);

 private:
  wxImage blackout_image;
  std::optional<Color> background_color;
  wxImage image;
  bool image_is_scaled;
  proto::ScreenSide screen_side;
  std::vector<proto::RenderableText> texts;

  ScreenText(wxWindow* parent, const wxString& initial_text,
             proto::ScreenSide side, wxSize size);
  ScreenText(wxWindow* parent, std::vector<proto::RenderableText> texts,
             wxImage image, std::optional<Color> background_color,
             proto::ScreenSide side, wxSize size);
  void bindEvents();
  wxPoint bottomText(wxDC& dc, wxString text);
  wxPoint centerText(wxDC& dc, wxString text);
  void initializeForColor(wxSize size, Color color);
  wxSize getTextExtent(wxDC& dc, wxString text);
  wxPoint positionText(wxDC& dc, proto::RenderableText text);
  float ratio(const wxSize& size);
  void renderBackground(wxDC& dc);
  void renderScaledBackground(wxDC& dc);
  void renderText(wxDC& dc, proto::RenderableText text);
  void renderAllText(wxDC& dc);
  void setBackground(const Color& color);
  wxPoint topText(wxDC& dc, wxString text);
};

}  // namespace cszb_scoreboard
