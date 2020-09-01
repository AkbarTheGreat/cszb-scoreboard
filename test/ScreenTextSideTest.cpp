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

  // Single line text
  wxSize size = screenText()->getTextExtent(dc, "Test Text");
  ASSERT_EQ(size.GetWidth(), 46) << "Single line text width does not match";
  ASSERT_EQ(size.GetHeight(), 15) << "Single line text height does not match";

  // Multi-line Text, with something on every line
  size = screenText()->getTextExtent(dc, "Test Text\n.\n.\n.End");
  ASSERT_EQ(size.GetWidth(), 46) << "Multi-line text with non-empty lines width does not match";
  ASSERT_EQ(size.GetHeight(), 60) << "Multi-line text with non-empty lines height does not match";

  // Multi-line Text, with blank lines
  size = screenText()->getTextExtent(dc, "Test Text\n\n\n.End");
  ASSERT_EQ(size.GetWidth(), 46) << "Multi-line text with empy lines width does not match";
  ASSERT_EQ(size.GetHeight(), 60) << "Multi-line text with empy lines height does not match";
}

}  // namespace test
}  // namespace cszb_scoreboard
