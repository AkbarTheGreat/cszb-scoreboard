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
#include "ui/TextEntry.h"

namespace cszb_scoreboard {
namespace test {

TEST_F(GuiTest, TextEntryTypingTest) {
  TextEntry *entry = mainView()->textEntry();
  WX_A(entry->textField()->SetFocus());
  WX_A(act.Text("."));
  // Always assume we're dealing with the left view, which is 1 in debug,
  // but we also assume that our tests only run on single-monitor.  So likely
  // this fails in non-debug at the moment.
  int preview_number = 0;
#ifdef WXDEBUG
  preview_number = 1;
#endif
  ImageAnalysis analysis(mainView()->preview(preview_number)->widget());
  float initial_text_ratio = analysis.colorPercentage(wxColour("White"));
  WX_A(act.Text("Longer Text"));
  analysis = ImageAnalysis(mainView()->preview(preview_number)->widget());
  float new_text_ratio = analysis.colorPercentage(wxColour("White"));
  // TODO: This test is broken, as the above act.Text() actions don't appear to
  // be firing correctly, or the SetFocus isn't working, one of the two.
  // ASSERT_LT(initial_text_ratio, new_text_ratio);
}

}  // namespace test
}  // namespace cszb_scoreboard
