/*
ui/component/control/TeamSelector.cpp: A group of radio buttons which selects
which team's or teams' screen(s) to send data to.

Copyright 2019-2022 Tracy Beck

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

#include "ui/component/control/TeamSelector.h"

#include <array>  // for array

#include "config.pb.h"         // for ScreenSide
#include "ui/widget/Widget.h"  // for NO_BORDER

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

static constexpr std::array<const char *, 3> CHOICES{{"Home", "Away", "All"}};

TeamSelector::TeamSelector(swx::Panel *wx) : Panel(wx) {
  selector = radio("Team", CHOICES.data(), CHOICES.size());
  addWidget(*selector, 0, 0, NO_BORDER);
  runSizer();
}

TeamSelector::TeamSelector(swx::Panel *wx, const proto::ScreenSide &side)
    : TeamSelector(wx) {
  if (side.home() && side.away()) {
    selector->setSelection(2);
  } else if (side.away()) {
    selector->setSelection(1);
  } else if (side.home()) {
    selector->setSelection(0);
  }
}

auto TeamSelector::allSelected() -> bool {
  return (selector->selection() == 2);
}

auto TeamSelector::awaySelected() -> bool {
  return (selector->selection() == 1 || selector->selection() == 2);
}

auto TeamSelector::homeSelected() -> bool {
  return (selector->selection() == 0 || selector->selection() == 2);
}

}  // namespace cszb_scoreboard
