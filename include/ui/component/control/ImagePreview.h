/*
ui/component/control/ImagePreview.h: A preview thumbnail of an image,
selectable to send to a ScreenText.

Copyright 2020 Tracy Beck

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

#include <wx/wx.h>

namespace cszb_scoreboard {

class ImagePreview : public wxPanel {
 public:
  ImagePreview(wxWindow* parent, const wxSize& size, const wxImage& image);
  ImagePreview(wxWindow* parent, const wxSize& size);

 private:
  void bindEvents();
  void paintEvent(wxPaintEvent& evt);
  float ratio(const wxSize& size);
  void renderImage(wxDC& dc);

  wxImage image;
  wxSize size;
};

}  // namespace cszb_scoreboard
