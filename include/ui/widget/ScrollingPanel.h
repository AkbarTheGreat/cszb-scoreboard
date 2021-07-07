/*
ui/widget/ScrollingPanel.h: A panel which may scroll around a larger area than
the panel itself.

Copyright 2021 Tracy Beck

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

#include "ui/widget/Panel.h"
#include "ui/widget/swx/ScrolledWindow.h"

namespace cszb_scoreboard {

class ScrollingPanel : public Panel {
 public:
  explicit ScrollingPanel(swx::ScrolledWindow *panel);

  void runSizer();

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::ScrolledWindow *wx;

 protected:
  [[nodiscard]] auto _wx() const -> wxWindow * override { return wx; }
};

}  // namespace cszb_scoreboard
