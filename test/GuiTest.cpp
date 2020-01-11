/*
test/GuiTest.cpp: A test utility for unit testing wxWidget functionality.  The
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

#include "test/GuiTest.h"

namespace cszb_scoreboard {
namespace test {

const int TEST_ARGC = 1;
const char *TEST_BIN_NAME = "scoreboardUnitTest.exe";
const char *TEST_ARGV[TEST_ARGC] = {TEST_BIN_NAME};

void GuiTest::SetUp() {
  app = new Scoreboard();
  wxApp::SetInstance(app);
  // Argument to wxEntryStart cannot be const, so copy to a non-const before
  // calling
  int argc = TEST_ARGC;
  wxEntryStart(argc, (char **)TEST_ARGV);
  app->OnInit();
  mainView()->Update();
}

void GuiTest::TearDown() {
  app->OnExit();
  wxEntryCleanup();
}

MainView *GuiTest::mainView() {
  return (MainView *)FrameList::getInstance()->getMainView();
}

TextEntry *GuiTest::textEntry() {
  return (TextEntry *)mainView()->controlPanel()->textController(0);
}

ScreenPreview *GuiTest::firstPreview() {
  return mainView()->previewPanel()->preview(0);
}

ImageAnalysis::ImageAnalysis(wxWindow *widget, int precision_percent) {
  wxClientDC dc(widget);
  wxRect dimensions = widget->GetRect();
  int pixel_step = 100 / precision_percent;
  for (int x = 0; x < dimensions.GetWidth(); x += pixel_step) {
    for (int y = 0; y < dimensions.GetHeight(); y += pixel_step) {
      wxColour color;
      dc.GetPixel(x, y, &color);
      unsigned int rgb = color.GetRGB();
      if (color_counts.find(rgb) == color_counts.end()) {
        color_counts[rgb] = 1;
      } else {
        color_counts[rgb]++;
      }
    }
  }

  int total = (dimensions.GetWidth() * dimensions.GetHeight()) / pixel_step;

  for (auto iterator : color_counts) {
    unsigned int color_rgb = iterator.first;
    float count = iterator.second;
    color_percentages[color_rgb] = count * 100 / total;
  }
}

float ImageAnalysis::colorPercentage(wxColour color) {
  if (color_percentages.find(color.GetRGB()) == color_percentages.end())
    return 0;
  return color_percentages[color.GetRGB()];
}

float ImageAnalysis::colorAmount(wxColour color) {
  if (color_counts.find(color.GetRGB()) == color_counts.end()) return 0;
  return color_counts[color.GetRGB()];
}

std::vector<int> ImageAnalysis::colorList() {
  std::vector<int> color_list;
  for (auto iterator = color_counts.begin(); iterator != color_counts.end();
       iterator++) {
    color_list.push_back(iterator->first);
  }
  return color_list;
}

}  // namespace test
}  // namespace cszb_scoreboard
