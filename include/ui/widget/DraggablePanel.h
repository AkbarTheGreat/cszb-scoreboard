/*
ui/widget/DraggablePanel.h: A panel where the contained objects are held by the
wxWidgets Advanced UI (AUI) manager.

Copyright 2021-2022 Tracy Beck

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

#include "ui/widget/Panel.h"  // for Panel

namespace cszb_scoreboard {
class Widget;
namespace swx {
class Panel;
}  // namespace swx

class DraggablePanel : public Panel {
 public:
  explicit DraggablePanel(swx::Panel *panel);
  ~DraggablePanel() override;
  void addWidget(const Widget &widget);
  void update() { aui_manager.Update(); }

 private:
  wxAuiManager aui_manager;
};

}  // namespace cszb_scoreboard
