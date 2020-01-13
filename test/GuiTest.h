/*
test/GuiTest.h: A test utility for unit testing wxWidget functionality.  The
idea for this methodology for testing wxWidgets via GoogleTest originally found
at http://www.remy.org.uk/tech.php?tech=1407951209 and rewritten here with
several improvements and modernizations.

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

#pragma once

#include <gtest/gtest.h>
#include <wx/uiaction.h>
#include <wx/wx.h>

#include <map>
#include <vector>

#include "cszb-scoreboard.h"
#include "ui/frame/FrameList.h"
#include "ui/frame/MainView.h"
#include "ui/component/ControlPanel.h"
#include "ui/component/ScreenPreview.h"

namespace cszb_scoreboard {
namespace test {

// Ultimately, I want to tune these so that they can be sped up.  But it doesn't
// work well yet, so call this a weirdly worded TODO.
#define FULL_TEST
#ifdef FULL_TEST
const int DEFAULT_IMAGE_ANALYSIS_PRECISION = 100;
#else
const int DEFAULT_IMAGE_ANALYSIS_PRECISION = 50;
#endif

/* Performs an action against the wxWidgets UI, and yields to allow it to
 * execute */
#define WX_A(action)      \
  {                       \
    action;               \
    wxYield();            \
    mainView()->Update(); \
  }

/* Checking if images are correct or not is tricky, so we have this to help */
class ImageAnalysis {
 public:
  ImageAnalysis(wxWindow *widget)
      : ImageAnalysis(widget, DEFAULT_IMAGE_ANALYSIS_PRECISION) {}
  // Create an ImageAnalysis object where only x% of the pixels are sampled for
  // color counts, for speed.
  ImageAnalysis(wxWindow *widget, int precision_percent);
  float colorPercentage(wxColour color);
  float colorAmount(wxColour color);
  std::vector<int> colorList();

 private:
  std::map<unsigned int, int> color_counts;
  std::map<unsigned int, float> color_percentages;
};

class GuiTest : public testing::Test {
 protected:
  Scoreboard *app;
  wxUIActionSimulator act;

  virtual void SetUp() override;
  virtual void TearDown() override;
  /* Convenience method to get various UI components, for testing purposes. */
  MainView *mainView();
  TextEntry *textEntry();
  /* Simply makes getting the first preview pane shorter in test code. */
  ScreenPreview *firstPreview();
};

}  // namespace test
}  // namespace cszb_scoreboard
