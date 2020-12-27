/*
test/ScreenTextSideTest.cpp: Tests for ui/component/ScreenTextSide

Copyright 2020 Tracy Beck

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

#include <wx/wx.h>

#include "config/TeamConfig.h"
#include "gtest/gtest.h"
#include "test/TestUtil.h"
#include "ui/component/ScreenTextSide.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {
namespace test {

class TestFrame : public wxFrame {
 public:
  ScreenTextSide *screen_text_side;

 public:
  TestFrame() : wxFrame(NULL, wxID_ANY, "ScreenTextSideTest") {
    screen_text_side =
        new ScreenTextSide(this, "Text", ProtoUtil::homeSide(), wxSize(50, 50));
    wxSizer *sizer = new wxFlexGridSizer(1, 1, 0, 0);
    sizer->Add(screen_text_side);
    this->SetSizerAndFit(sizer);
  }
};

class TestApp : public wxApp {
 public:
  TestFrame *frame;

  virtual bool OnInit() {
    frame = new TestFrame();
    frame->Show(true);
    return true;
  }
};

class ScreenTextSideTest : public testing::Test {
 protected:
  TestApp *app;

  ScreenTextSide *screenText() { return app->frame->screen_text_side; }

  virtual void SetUp() override {
    app = new TestApp();
    TestUtil::startApp(app);
  }

  virtual void TearDown() {
    app->OnExit();
    wxEntryCleanup();
  }
};

TEST_F(ScreenTextSideTest, getExtents) {
  wxClientDC dc(screenText());

  wxSize size = screenText()->getTextExtent(&dc, "Test Text");
  int line_width = size.GetWidth();
  int line_height = size.GetHeight();

  // Single line text -- we can't guarantee pixel sizes per-platform, so let's
  // get a baseline here.
  ASSERT_GT(line_width, 35) << "Single line text width looks unrealistic";
  ASSERT_GT(line_height, 10) << "Single line text height looks unrealistic";

  // Multi-line Text, with something on every line
  size = screenText()->getTextExtent(&dc, "Test Text\n.\n.\nEnd");
  ASSERT_EQ(size.GetWidth(), line_width)
      << "Multi-line text with non-empty lines width does not match";
  ASSERT_EQ(size.GetHeight(), line_height*4)
      << "Multi-line text with non-empty lines height does not match";

  // Multi-line Text, with blank lines
  size = screenText()->getTextExtent(&dc, "Test Text\n\n\nEnd");
  ASSERT_EQ(size.GetWidth(), line_width)
      << "Multi-line text with empty lines width does not match";
  ASSERT_EQ(size.GetHeight(), line_height*4)
      << "Multi-line text with empty lines height does not match";

  // Multi-line Text, with trailing blank lines
  size = screenText()->getTextExtent(&dc, "Test Text\n\n\n\n");
  ASSERT_EQ(size.GetWidth(), line_width)
      << "Multi-line text with trailing empty lines width does not match";
  ASSERT_EQ(size.GetHeight(), line_height*5)
      << "Multi-line text with trailing empty lines height does not match";
}

}  // namespace test
}  // namespace cszb_scoreboard
