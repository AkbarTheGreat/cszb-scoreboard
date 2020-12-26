/*
ui/component/PreviewPanel.h: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

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

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include <vector>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "ui/component/ScreenPreview.h"
#include "ui/component/ScreenTextSide.h"

namespace cszb_scoreboard {

class PreviewPanel : public wxPanel {
 public:
  explicit PreviewPanel(wxWindow* parent);
  void blackout(wxCommandEvent& event); // NOLINT(google-runtime-references) wxWidgets callback.
  inline auto all_screens() -> std::vector<ScreenPreview*> { return screens; }
  void setToPresenters(ScreenText* screen_text);
  void updatePresenters();
  void updatePreviewsFromSettings();

  PUBLIC_TEST_ONLY
  auto preview(int index) -> ScreenPreview*;

 private:
  ~PreviewPanel() override;
  wxAuiManager aui_manager;
  // Contains a view of the screens, does not own the screens themselves.
  std::vector<ScreenPreview*> screens;
  void bindEvents();
  auto numPreviews() -> int;
  void positionWidgets();
};

}  // namespace cszb_scoreboard
