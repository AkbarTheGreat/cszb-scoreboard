/*
test/ScreenPreviewTest.cpp: Tests for ui/ScreenPreview

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

#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>

#include <vector>

#include "config.pb.h"
#include "config/DisplayConfig.h"
#include "gtest/gtest_pred_impl.h"
#include "test/integration/GuiTest.h"
#include "ui/component/ScreenPreview.h"
#include "ui/component/ScreenText.h"
#include "ui/component/ScreenTextSide.h"
#include "ui/component/control/TextEntry.h"
#include "ui/frame/MainView.h"
#include "wx/colour.h"

namespace cszb_scoreboard::test {

void assertErrorColors(const ImageAnalysis &analysis) {
  // The error image on pane 0 is ~50% white, ~50% red with black text over
  // part of it.  We rely on display config to tell us which kind of display
  // we should be testing.
  ASSERT_GT(analysis.colorPercentage(wxColour("Red")), 25);
  ASSERT_LT(analysis.colorPercentage(wxColour("Red")), 70);
  ASSERT_GT(analysis.colorPercentage(wxColour("White")), 25);
  ASSERT_LT(analysis.colorPercentage(wxColour("White")), 70);
  ASSERT_GT(analysis.colorPercentage(wxColour("Black")), 5);
  ASSERT_LT(analysis.colorPercentage(wxColour("Black")), 20);
}

void assertTeamColors(const ImageAnalysis &analysis) {
  ASSERT_GT(analysis.colorPercentage(wxColour("Blue")), 70);
  ASSERT_GT(analysis.colorPercentage(wxColour("White")), 1.25);
  ASSERT_LT(analysis.colorPercentage(wxColour("White")), 7);
}

TEST_F(GuiTest, ScreenPreviewInitializationTest) {
  // Probably unnecessary to set the focus, but doing it anyway
  WX_A(mainView()->focus());
  WX_A(textEntry()->selectTeam(0));
  ImageAnalysis analysis(firstPreview()->screen()->sidePanel(0),
                         IA_MODE_QUARTER_SCAN);
  if (Singleton::getInstance()
          ->displayConfig()
          ->displayDetails(0)
          .side()
          .error()) {
    assertErrorColors(analysis);
  } else {
    assertTeamColors(analysis);
  }
}

}  // namespace cszb_scoreboard::test
