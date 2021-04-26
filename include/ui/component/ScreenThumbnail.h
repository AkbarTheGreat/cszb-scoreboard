/*
ui/component/ScreenThumbnail.h: Represents a very small view of exactly what's
on the matching ScreenPresenter, in case there's not a great view from the booth
to all monitors.

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

#include <wx/wx.h>

#include "ui/component/ScreenText.h"

namespace cszb_scoreboard {
class ScreenThumbnail {
 public:
  ScreenThumbnail(wxWindow* parent, int monitor_number, ScreenText* widget);
  inline auto widget() -> ScreenText* { return screen_text; }

 private:
  // Just holding a pointer to this wxWidget, this object does not own it.
  ScreenText* screen_text;
  static auto thumbnailSize(int monitor_number) -> wxSize;
};
}  // namespace cszb_scoreboard
