/*
ui/ScreenPresenter.h: This class manages any content which is being
displayed on an external screen.

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

#include <wx/wx.h>

#include "ui/ScreenText.h"

namespace cszb_scoreboard {
class ScreenPresenter : public wxFrame {
 public:
  ScreenPresenter(int monitor_number, ScreenText *widget);
  inline ScreenText *widget() { return screen_text; }

 private:
  int monitor_number;
  ScreenText *screen_text;
  void positionWidgets();
};

}  // namespace cszb_scoreboard
