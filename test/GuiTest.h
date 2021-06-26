/*
test/GuiTest.h: A test utility for unit testing wxWidget functionality.  The
idea for this methodology for testing wxWidgets via GoogleTest originally found
at http://www.remy.org.uk/tech.php?tech=1407951209 and rewritten here with
several improvements and modernizations.

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

#pragma once

#include <gtest/gtest.h>
#include <wx/uiaction.h>
#include <wx/wx.h>

#include <map>
#include <vector>

#include "cszb-scoreboard.h"
#include "ui/component/ControlPanel.h"
#include "ui/component/ScreenPreview.h"
#include "ui/frame/MainView.h"

namespace cszb_scoreboard::test {

/* Performs an action against the wxWidgets UI, and yields to allow it to
 * execute */
#define WX_A(action)      \
  {                       \
    action;               \
    wxYield();            \
    mainView()->Update(); \
  }

enum ImageAnalysisMode {
  IA_MODE_FULL_SCAN,
  IA_MODE_CENTERLINE_SCAN,
  IA_MODE_QUARTER_SCAN,
};

/* Checking if images are correct or not is tricky, so we have this to help */
class ImageAnalysis {
 public:
  explicit ImageAnalysis(wxWindow *widget)
      : ImageAnalysis(widget, IA_MODE_FULL_SCAN) {}
  // Create an ImageAnalysis object where only x% of the pixels are sampled for
  // color counts, for speed.
  ImageAnalysis(wxWindow *widget, ImageAnalysisMode scan_mode);
  auto colorPercentage(const wxColour &color) -> float;
  auto colorAmount(const wxColour &color) -> float;
  auto colorList() -> std::vector<int>;

 private:
  std::map<unsigned int, int> color_counts;
  std::map<unsigned int, float> color_percentages;
  void countAllPixels(const wxClientDC &dc, const wxRect &dimensions);
  void countCenterlinePixels(const wxClientDC &dc, const wxRect &dimensions);
  void countQuarterScanlinePixels(const wxClientDC &dc,
                                  const wxRect &dimensions);
};

class GuiTest : public testing::Test {
 protected:
  Scoreboard *app;
  wxUIActionSimulator act;

  void SetUp() override;
  void TearDown() override;
  /* Convenience method to get various UI components, for testing purposes. */
  static auto mainView() -> MainView *;
  static auto textEntry() -> TextEntry *;
  /* Simply makes getting the first preview pane shorter in test code. */
  static auto firstPreview() -> ScreenPreview *;
};

}  // namespace cszb_scoreboard::test
