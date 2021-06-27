/*
ui/widget/Frame.cpp: A frame represents a free-standing window that is not a
dialog.

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

#include "ui/widget/Frame.h"

namespace cszb_scoreboard {

void Frame::setStatusBar(const wxString &text) {
  if (!hasStatusBar) {
    hasStatusBar = true;
    wx->CreateStatusBar();
  }
  wx->SetStatusText(text);
}

}  // namespace cszb_scoreboard
