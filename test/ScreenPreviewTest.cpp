/*
test/ScreenPreviewTest.cpp: Tests for ui/ScreenPreview

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

#include "test/GuiTest.h"

namespace cszb_scoreboard {

TEST_F(GuiTest, InitializationTest) {
  // Probably unnecessary to set the focus, but doing it anyway
  WX_A(mainView()->SetFocus());
  wxClientDC preview_dc(mainView()->preview(0)->widget());
  wxColour origin_color;
  preview_dc.GetPixel(0, 0, &origin_color);
  ASSERT_EQ(wxColor("Red"), origin_color);
}
}  // namespace cszb_scoreboard
