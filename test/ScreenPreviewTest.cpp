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
namespace test {

TEST_F(GuiTest, ScreenPreviewInitializationTest) {
  // Probably unnecessary to set the focus, but doing it anyway
  WX_A(mainView()->SetFocus());
  ImageAnalysis analysis(mainView()->preview(0)->widget());
  std::vector<int> color_list = analysis.colorList();
  int list_size = color_list.size();
  // The error image on pane 0 is ~50% white, ~50% red with black text over part
  // of it.  This test will fail if run on a machine with multiple monitors.  If
  // that comes up, fix this test, I guess.
  ASSERT_GT(analysis.colorPercentage(wxColour("Red")), 40);
  ASSERT_GT(analysis.colorPercentage(wxColour("White")), 40);
  ASSERT_GT(analysis.colorPercentage(wxColour("Black")), 5);
}

}  // namespace test
}  // namespace cszb_scoreboard
