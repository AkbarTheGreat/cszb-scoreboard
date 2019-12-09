/*
ui/ScreenPreview.cpp: This class manages the thumbnail preview of a monitor
in the main window.  In addition, the preview owns the ScreenPresenter which
displays to the actual monitor, dispatching the updated view when necessary.

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
#include "ui/ScreenPreview.h"
#include "ui/ScreenText.h"

namespace cszb_scoreboard {

const char* WELCOME_MESSAGE = "Chandler";
const char* ERROR_MESSAGE = "NO SCREENS FOUND!";

ScreenPreview::ScreenPreview(wxWindow* parent, ScreenSide side) {
  this->parent = parent;
  ScreenText* screen_text = nullptr;
  switch (side) {
    case SIDE_NONE:
      screen_text = ScreenText::getPreview(parent, ERROR_MESSAGE, side);
      break;
    default:
      screen_text = ScreenText::getPreview(parent, WELCOME_MESSAGE, side);
      presenter = new ScreenPresenter(
          0, side,
          screen_text);  // TODO: Pass along screen number that's expected
  }
  this->current_widget = screen_text;
}

wxPanel* ScreenPreview::widget() { return current_widget; }

}  // namespace cszb_scoreboard
