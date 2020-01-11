/*
ui/component/ScreenText.h: Represents a text presentation on a ScreenPresenter
or a ScreenPreview.

Copyright 2019 Tracy Beck

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

#include "proto/config.pb.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

class RenderableText {
 public:
  RenderableText(wxString text, float font_size);
  RenderableText(wxString text, proto::Font font);
  void setFont(float font_size);
  void setFontColor(Color color);
  void setText(const wxString& text, int font_size);
  wxString getText();
  proto::Font getFont();

 private:
  wxString text;
  proto::Font font;
};

class ScreenText : public wxPanel {
 public:
  // These two methods are really just light wrappers around constructors, but
  // they help document the use each constructor is intended to fill.
  static ScreenText* getPreview(wxWindow* parent, wxString initial_text,
                                proto::ScreenSide side, wxSize size);

  static ScreenText* getPresenter(wxWindow* parent, ScreenText* preview,
                                  wxSize size);

  void blackout();
  void paintEvent(wxPaintEvent& event);
  void setImage(const wxImage& image);
  void setBackground(const Color color);
  void setText(const wxString& text, int font_size,
               const proto::ScreenSide& side);
  void setAll(const ScreenText& source);
  bool isSide(proto::ScreenSide side);

 private:
  wxImage image;
  wxImage blackout_image;
  std::optional<Color> background_color;
  proto::ScreenSide screen_side;
  std::vector<RenderableText> texts;

  ScreenText(wxWindow* parent, const wxString& initial_text,
             proto::ScreenSide side, wxSize size);
  ScreenText(wxWindow* parent, std::vector<RenderableText> texts, wxImage image,
             std::optional<Color> background_color, proto::ScreenSide side,
             wxSize size);
  void bindEvents();
  void initializeForColor(wxSize size, Color color);
  void getTextExtent(wxDC& dc, wxString text, int* width, int* height);
  void renderBackground(wxDC& dc);
  void renderText(wxDC& dc, RenderableText text);
  void renderAllText(wxDC& dc);
  void resetAllText();
  void addText(const wxString& text, proto::Font font,
               const proto::ScreenSide& side);
};

}  // namespace cszb_scoreboard
