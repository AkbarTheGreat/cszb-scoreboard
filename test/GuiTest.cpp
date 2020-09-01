/*
test/GuiTest.cpp: A test utility for unit testing wxWidget functionality.  The
idea for this methodology for testing wxWidgets via GoogleTest originally found
at http://www.remy.org.uk/tech.php?tech=1407951209 and rewritten here with
several improvements and modernizations.

Copyright 2019-2020 Tracy Beck

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

#include "test/TestUtil.h"

namespace cszb_scoreboard {
namespace test {

const int TEXT_ENTRY_TAB_INDEX = 4;

void GuiTest::SetUp() {
  app = new Scoreboard();
  TestUtil::startApp(app);
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
  mainView()->controlPanel()->SetSelection(TEXT_ENTRY_TAB_INDEX);
  return (TextEntry *)mainView()->controlPanel()->textController(
      TEXT_ENTRY_TAB_INDEX);
}

ScreenPreview *GuiTest::firstPreview() {
  return mainView()->previewPanel()->preview(0);
}

ImageAnalysis::ImageAnalysis(wxWindow *widget, ImageAnalysisMode scan_mode) {
  wxClientDC dc(widget);
  wxRect dimensions = widget->GetRect();

  int total = 1;

  switch (scan_mode) {
    case IA_MODE_CENTERLINE_SCAN:
      countCenterlinePixels(dc, dimensions);
      total = dimensions.GetWidth();
      break;
    case IA_MODE_QUARTER_SCAN:
      countQuarterScanlinePixels(dc, dimensions);
      total = dimensions.GetWidth() * 5;
      break;
    default:
      countAllPixels(dc, dimensions);
      total = dimensions.GetWidth() * dimensions.GetHeight();
  }

  for (auto iterator : color_counts) {
    unsigned int color_rgb = iterator.first;
    float count = iterator.second;
    color_percentages[color_rgb] = count * 100 / total;
  }
}

void ImageAnalysis::countAllPixels(const wxClientDC &dc,
                                   const wxRect &dimensions) {
  for (int x = 0; x < dimensions.GetWidth(); ++x) {
    for (int y = 0; y < dimensions.GetHeight(); ++y) {
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
}

void ImageAnalysis::countQuarterScanlinePixels(const wxClientDC &dc,
                                               const wxRect &dimensions) {
  int quarter_step = dimensions.GetHeight() / 4;
  for (int y = 0; y < dimensions.GetHeight(); y += quarter_step) {
    for (int x = 0; x < dimensions.GetWidth(); ++x) {
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
}

void ImageAnalysis::countCenterlinePixels(const wxClientDC &dc,
                                          const wxRect &dimensions) {
  int y = dimensions.GetHeight() / 2;
  for (int x = 0; x < dimensions.GetWidth(); ++x) {
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
