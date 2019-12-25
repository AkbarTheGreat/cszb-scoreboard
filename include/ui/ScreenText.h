/*
ui/ScreenText.h: Represents a text presentation on a ScreenProvider.

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

#include "proto/config.pb.h"
#include "ui/Color.h"

namespace cszb_scoreboard {

class ScreenText : public wxPanel {
 public:
  static ScreenText* getPreview(wxWindow* parent, float ratio,
                                proto::ScreenSide side);

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
  wxString text;
  std::optional<Color> background_color;
  Color font_color;
  float font_size = 10;
  proto::ScreenSide screen_side;

  ScreenText(wxWindow* parent, const wxString& initial_text,
             proto::ScreenSide side, wxSize size);
  ScreenText(wxWindow* parent, const wxString& initial_text, wxImage image,
             std::optional<Color> background_color, Color font_color,
             proto::ScreenSide side, wxSize size);
  void bindEvents();
  void initializeForColor(wxSize size, Color color);
  void getTextExtent(wxDC& dc, wxString text, int* width, int* height);
  void renderBackground(wxDC& dc, wxImage image);
  void renderText(wxDC& dc, wxString text, Color font_color,
                  wxSize widget_size);
  int scaleFont(wxSize to_size);
};

}  // namespace cszb_scoreboard
