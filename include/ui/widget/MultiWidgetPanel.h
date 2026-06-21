/*
ui/widget/MultiWidgetPanel.h: A panel which contains multiple widgets, but only
one is visible at a time.

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

#pragma once

#include "ui/widget/Panel.h"               // for Panel
#include "ui/widget/swx/ScrolledWindow.h"  // for ScrolledWindow

class wxWindow;

namespace cszb_scoreboard {

class MultiWidgetPanel : public Panel {
 public:
  explicit MultiWidgetPanel(swx::Panel* panel);

  void addWidget(const Widget& widget);
  void hideAllWidgets();
  void showWidget(const Widget& widget);
  void runSizer();
};

}  // namespace cszb_scoreboard
