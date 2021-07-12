/*
test/ScreenPreviewTest.cpp: Tests for ui/ScreenPreview

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

#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <wx/uiaction.h>
#include <memory>

#include "config/DisplayConfig.h"
#include "test/GuiTest.h"
#include "ui/component/control/TextEntry.h"
#include "config.pb.h"
#include "gtest/gtest_pred_impl.h"
#include "ui/component/ScreenPreview.h"
#include "ui/component/ScreenText.h"
#include "ui/component/ScreenTextSide.h"
#include "ui/widget/Text.h"
#include "wx/colour.h"

namespace cszb_scoreboard ::test {

class TextEntryTest : public GuiTest {
 protected:
  static void enterTextIndirect(const char *text) {
    TextEntry *entry = textEntry();
    WX_A(entry->textField()->focus());
    WX_A(entry->textField()->clear());
    WX_A(entry->textField()->setValue(text));
    WX_A(entry->textUpdated());
  }

  void enterTextDirect(const char *text) {
    TextEntry *entry = textEntry();
    WX_A(entry->textField()->focus());
    WX_A(entry->textField()->clear());
    WX_A(act.Text(text));
  }

  static void enterText(const char *text) {
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

  static auto getAnalysis() -> ImageAnalysis {
    // Always assume we're dealing with the left (home) view, which is 0 in most
    // cases, but may also be an error screen in release, so this will fail for
    // release at the moment.
    return ImageAnalysis(firstPreview()->screen()->sidePanel(0),
                         IA_MODE_CENTERLINE_SCAN);
  }
};

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(TextEntryTest, TypingTest) {
  if (DisplayConfig::getInstance()->displayDetails(0).side().error()) {
    // We can't hope to meaningfully test text entry with no screens that can
    // accept text entry, so just pass spuriously.
    return;
  }

  WX_A(textEntry()->selectTeam(0));

  wxColour text_color = wxColour("White");
  enterText(".");
  ImageAnalysis analysis = getAnalysis();
  float initial_text_ratio = analysis.colorPercentage(text_color);
  enterText("Longer Text");
  analysis = getAnalysis();
  float new_text_ratio = analysis.colorPercentage(text_color);
  ASSERT_LT(initial_text_ratio, new_text_ratio);
}

}  // namespace cszb_scoreboard::test
