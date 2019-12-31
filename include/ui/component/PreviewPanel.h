/*
ui/component/PreviewPanel.h: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

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

#include <vector>

#include "proto/config.pb.h"
#include "ui/component/ScreenPreview.h"

namespace cszb_scoreboard {

class PreviewPanel : public wxPanel {
 public:
  PreviewPanel(wxWindow* parent);
  void blackout(wxCommandEvent& event);
  void setTextForPreview(wxString text, int font_size, proto::ScreenSide side);
  void updatePresenters(proto::ScreenSide side);
  void updatePreviewsFromSettings();

 private:
  std::vector<ScreenPreview*> screens;
  void bindEvents();
  int numPreviews();
  void positionWidgets();
  ScreenPreview* preview(int index);
};

}  // namespace cszb_scoreboard
