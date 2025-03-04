/*
ui/widget/DraggablePanel.cpp: A panel where the contained objects are held by
the wxWidgets Advanced UI (AUI) manager.

Copyright 2021-2025 Tracy Beck

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

#include "ui/widget/DraggablePanel.h"

#include "ui/widget/Widget.h"  // for Widget
#include "wx/window.h"         // for wxWindow

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

DraggablePanel::DraggablePanel(swx::Panel *panel) : Panel(panel) {
  aui_manager.SetManagedWindow(wx());
}

DraggablePanel::~DraggablePanel() { aui_manager.UnInit(); }

void DraggablePanel::addWidget(const Widget &widget) {
  wxAuiPaneInfo pane_style;
  pane_style.CenterPane();
  pane_style.Top();
  pane_style.CloseButton(false);
  pane_style.MinSize(widget.wx()->GetSize());

  aui_manager.AddPane(widget.wx(), pane_style);
}

}  // namespace cszb_scoreboard
