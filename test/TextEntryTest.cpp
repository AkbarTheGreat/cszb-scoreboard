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
#include "ui/component/TextEntry.h"

namespace cszb_scoreboard {
namespace test {

class TextEntryTest : public GuiTest {
 protected:
  void enterTextIndirect(const char *text) {
    TextEntry *entry = mainView()->textEntry();
    WX_A(entry->textField()->SetFocus());
    WX_A(entry->textField()->Clear());
    WX_A(entry->textField()->WriteText(text));
    WX_A(entry->textUpdated(wxKeyEvent()));
  }

  void enterTextDirect(const char *text) {
    TextEntry *entry = mainView()->textEntry();
    WX_A(entry->textField()->SetFocus());
    WX_A(entry->textField()->Clear());
    WX_A(act.Text(text));
  }

  void enterText(const char *text) {
    // wxUIActionSimulator has problems described at
    // http://trac.wxwidgets.org/ticket/17400 which implies that there are
    // issues in some unkown circumstances? Maybe we're better off avoiding it
    // anyway, but we can revisit if that bug gets closed.
#ifdef UIACTIONSIMULATOR_TESTING
    enterTextDirect(text);
#else
    enterTextIndirect(text);
#endif
  }

  ImageAnalysis getAnalysis() {
    // Always assume we're dealing with the left (home) view, which is 0 in most
    // cases, but may also be an error screen in release, so this will fail for
    // release at the moment.
    return ImageAnalysis(mainView()->preview(0)->widget());
  }
};

TEST_F(TextEntryTest, TypingTest) {
  wxColour text_color = wxColour("Black");
#ifdef WXDEBUG
  text_color = wxColour("White");
#endif
  enterText(".");
  ImageAnalysis analysis = getAnalysis();
  float initial_text_ratio = analysis.colorPercentage(text_color);
  enterText("Longer Text");
  analysis = getAnalysis();
  float new_text_ratio = analysis.colorPercentage(text_color);
  // TODO: This test is broken, as the above act.Text() actions don't appear to
  // be firing correctly, or the SetFocus isn't working, one of the two.
  ASSERT_LT(initial_text_ratio, new_text_ratio);
}

}  // namespace test
}  // namespace cszb_scoreboard
