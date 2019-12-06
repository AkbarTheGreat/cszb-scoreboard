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
#include "ui/ScreenSide.h"
#include "ui/Color.h"

namespace cszb_scoreboard {

class ScreenText : public wxPanel {
 private:
  wxImage image;
  wxString text;
  ScreenSide side;
  Color fontColor;
  ScreenText(wxWindow* parent, const wxString& initialText, ScreenSide side,
             wxSize size);

 public:
  static ScreenText* GetPreview(wxWindow* parent, const wxString& initialText,
                                ScreenSide side);

  static ScreenText* GetPresenter(wxWindow* parent, ScreenText* preview,
                                  wxSize size);

  void paintEvent(wxPaintEvent& evt);
  inline void setImage(const wxImage& image) { this->image = image; };
  inline void setText(const wxString& text) { this->text = text; };
  DECLARE_EVENT_TABLE()
};

}  // namespace cszb_scoreboard
