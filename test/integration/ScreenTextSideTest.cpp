/*
test/ScreenTextSideTest.cpp: Tests for ui/component/ScreenTextSide

Copyright 2020-2023 Tracy Beck

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

#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult, SuiteApiRes...
#include <gtest/gtest.h>            // IWYU pragma: keep
#include <wx/app.h>                 // IWYU pragma: keep for wxApp
#include <wx/init.h>                // for wxEntryCleanup

#include <memory>  // for unique_ptr, allocator, make...

#include "config/Position.h"              // for Size
#include "test/integration/GuiTest.h"     // for GuiTest
#include "ui/component/ScreenTextSide.h"  // for ScreenTextSide
#include "ui/widget/Frame.h"              // for Frame
#include "ui/widget/RenderContext.h"      // for RenderContext
#include "ui/widget/swx/Panel.h"          // for Panel
#include "util/ProtoUtil.h"               // for ProtoUtil
// IWYU pragma: no_include <wx/gtk/app.h>
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard ::test {

static const int STARTING_WIDTH = 50;
static const int STARTING_HEIGHT = 50;

class TestFrame : public Frame {
 public:
  std::unique_ptr<ScreenTextSide> screen_text_side;

  TestFrame() : Frame("ScreenTextSideTest") {
    screen_text_side = std::make_unique<ScreenTextSide>(
        childPanel(), "Text", ProtoUtil::homeSide(),
        Size{.width = STARTING_WIDTH, .height = STARTING_HEIGHT});
    addWidget(*screen_text_side, 0, 0);
    runSizer();
  }
};

class TestApp : public wxApp {
 public:
  TestFrame *frame;

  auto OnInit() -> bool override {
    frame = new TestFrame();
    frame->show(true);
    return true;
  }
};

class ScreenTextSideTest : public testing::Test {
 protected:
  TestApp *app;

  auto screenText() -> ScreenTextSide * {
    return app->frame->screen_text_side.get();
  }

  void SetUp() override {
    app = new TestApp();
    GuiTest::startApp(app);
  }

  void TearDown() override {
    app->OnExit();
    wxEntryCleanup();
  }
};

TEST_F(ScreenTextSideTest, getExtents) {
  std::unique_ptr<RenderContext> renderer =
      RenderContext::forWidget(screenText()->wx());

  Size size = ScreenTextSide::getTextExtent(renderer.get(), "Test Text");
  int line_width = size.width;
  int line_height = size.height;

  // Single line text -- we can't guarantee pixel sizes per-platform, so let's
  // get a baseline here.
  ASSERT_GT(line_width, 35) << "Single line text width looks unrealistic";
  ASSERT_GT(line_height, 10) << "Single line text height looks unrealistic";

  // Multi-line Text, with something on every line
  size = ScreenTextSide::getTextExtent(renderer.get(), "Test Text\n.\n.\nEnd");
  ASSERT_EQ(size.width, line_width)
      << "Multi-line text with non-empty lines width does not match";
  ASSERT_EQ(size.height, line_height * 4)
      << "Multi-line text with non-empty lines height does not match";

  // Multi-line Text, with blank lines
  size = ScreenTextSide::getTextExtent(renderer.get(), "Test Text\n\n\nEnd");
  ASSERT_EQ(size.width, line_width)
      << "Multi-line text with empty lines width does not match";
  ASSERT_EQ(size.height, line_height * 4)
      << "Multi-line text with empty lines height does not match";

  // Multi-line Text, with trailing blank lines
  size = ScreenTextSide::getTextExtent(renderer.get(), "Test Text\n\n\n\n");
  ASSERT_EQ(size.width, line_width)
      << "Multi-line text with trailing empty lines width does not match";
  ASSERT_EQ(size.height, line_height * 5)
      << "Multi-line text with trailing empty lines height does not match";
}

}  // namespace cszb_scoreboard::test
