/*
ui/component/control/ScoreControl.h: This class is responsible for generating a
score update which is sent to all scoreboard screens.

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

#include "ui/component/ScreenTextController.h"

namespace cszb_scoreboard {

class ScoreControl : public ScreenTextController {
 public:
  static TextEntry *Create(PreviewPanel *preview_panel, wxWindow *parent) {
    return nullptr;
  }

 private:
  ScoreControl(PreviewPanel *preview_panel, wxWindow *parent)
      : ScreenTextController(preview_panel, parent) {}
  void updatePreview() override {}
  void createControls(wxPanel *control_panel) override {}
  proto::ScreenSide updateSide() override { return proto::ScreenSide(); }
};

}  // namespace cszb_scoreboard
