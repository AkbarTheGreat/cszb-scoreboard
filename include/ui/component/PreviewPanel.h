/*
ui/component/PreviewPanel.h: The panel which holds all of the screen previews
(which then, in turn hold the screen presenters which control the monitors
themselves).

Copyright 2019-2024 Tracy Beck

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

#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <vector>      // for vector

#include "ScoreboardCommon.h"            // for PUBLIC_TEST_ONLY
#include "ui/component/ScreenPreview.h"  // for ScreenPreview
#include "ui/widget/DraggablePanel.h"    // for DraggablePanel
#include "util/Singleton.h"

namespace cszb_scoreboard {
class ScreenText;

namespace swx {
class Panel;
}  // namespace swx

class PreviewPanel : public DraggablePanel {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  explicit PreviewPanel(swx::Panel *wx)
      : PreviewPanel(wx, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP

  void blackout();
  void forAllScreens(const std::function<void(ScreenPreview *)> &lambda);
  void setToPresenters(ScreenText *screen_text);
  void updatePresenters();
  void updatePreviewsFromSettings();

  PUBLIC_TEST_ONLY
  explicit PreviewPanel(swx::Panel *wx, Singleton *singleton);
  auto preview(int index) -> ScreenPreview *;

 private:
  std::vector<std::unique_ptr<ScreenPreview>> screens;
  Singleton *singleton;
  auto numPreviews() -> int;
  void positionWidgets();
};

}  // namespace cszb_scoreboard
