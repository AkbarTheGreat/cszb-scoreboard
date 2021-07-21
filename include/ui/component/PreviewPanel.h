/*
ui/component/PreviewPanel.h: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

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

#include <wx/aui/framemanager.h>  // for wxAuiManager

#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <vector>      // for vector

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "ui/widget/Panel.h"   // for Panel

class wxCommandEvent;

namespace cszb_scoreboard {
class ScreenPreview;
class ScreenText;

namespace swx {
class Panel;
}  // namespace swx

class PreviewPanel : public Panel {
 public:
  explicit PreviewPanel(swx::Panel *wx);
  ~PreviewPanel();

  void blackout();
  void forAllScreens(const std::function<void(ScreenPreview *)> &lambda);
  void setToPresenters(ScreenText *screen_text);
  void updatePresenters();
  void updatePreviewsFromSettings();

  PUBLIC_TEST_ONLY
  auto preview(int index) -> ScreenPreview *;

 private:
  wxAuiManager aui_manager;
  // Contains a view of the screens, does not own the screens themselves.
  std::vector<std::unique_ptr<ScreenPreview>> screens;
  auto numPreviews() -> int;
  void positionWidgets();
};

}  // namespace cszb_scoreboard
