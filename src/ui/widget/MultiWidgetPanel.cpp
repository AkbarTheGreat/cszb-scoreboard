/*
ui/widget/MultiWidgetPanel.cpp: A panel which contains multiple widgets, but
only one is visible at a time.

Copyright 2026 Tracy Beck

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

#include "ui/widget/MultiWidgetPanel.h"

#include <list>  // for operator==, _List_iterator

#include "ScoreboardCommon.h"              // for DEFAULT_BORDER_SIZE
#include "ui/widget/Widget.h"              // for Widget
#include "ui/widget/swx/SwappableSizer.h"  // for SwappableSizer
#include "wx/defs.h"                       // for operator|, wxDirection
#include "wx/sizer.h"                      // for wxSizerItemList, wxSizerItem
#include "wx/window.h"                     // for wxWindow

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

MultiWidgetPanel::MultiWidgetPanel(swx::Panel* panel) : Panel(panel) {}

void MultiWidgetPanel::addWidget(const Widget& widget) {
  widget.hide();
  swappable_sizer()->Add(widget.wx(), 0, wxALL | wxGROW, DEFAULT_BORDER_SIZE);
}

void MultiWidgetPanel::hideAllWidgets() {
  for (auto* item : swappable_sizer()->GetChildren()) {
    if (item->IsWindow() && item->GetWindow() != nullptr) {
      item->GetWindow()->Hide();
    }
  }
}

void MultiWidgetPanel::showWidget(const Widget& widget) {
  hideAllWidgets();
  widget.show();
}

void MultiWidgetPanel::runSizer() {
  for (auto* item : swappable_sizer()->GetChildren()) {
    if (item->IsWindow() && item->GetWindow() != nullptr) {
      item->GetWindow()->Layout();
    }
  }
  wx()->SetSizerAndFit(swappable_sizer());
}

}  // namespace cszb_scoreboard
