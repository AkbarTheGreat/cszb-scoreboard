/*
test/GuiTest.cpp: A test utility for unit testing wxWidget functionality.  The
idea for this methodology for testing wxWidgets via GoogleTest originally found
at http://www.remy.org.uk/tech.php?tech=1407951209 and rewritten here with
several improvements and modernizations.

Copyright 2019-2026 Tracy Beck

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

#include "test/integration/GuiTest.h"

#include <wx/dcmemory.h>  // for wxMemoryDC
#include <wx/init.h>      // for wxEntryCleanup, wxEntryS...

#include <array>    // for array
#include <memory>   // for unique_ptr
#include <utility>  // for pair

#include "config/swx/image.h"                // for Image
#include "cszb-scoreboard.h"                 // for Scoreboard
#include "ui/component/PreviewPanel.h"       // for PreviewPanel
#include "ui/component/ScreenTextSide.h"     // for ScreenTextSide
#include "ui/component/control/TextEntry.h"  // for TextEntry
#include "ui/frame/FrameManager.h"           // for FrameManager
#include "ui/frame/MainView.h"               // for MainView
#include "ui/widget/Panel.h"                 // for Panel
#include "ui/widget/RenderContext.h"         // for RenderContext
#include "util/Singleton.h"                  // for Singleton
#include "wx/bitmap.h"                       // for wxBitmap
#include "wx/dcclient.h"                     // for wxClientDC
#include "wx/defs.h"                         // for wxSetAssertHandler
#include "wx/gdicmn.h"                       // for wxRect, wxNullBitmap
#include "wx/window.h"                       // for wxWindow
// IWYU pragma: no_include "wx/gtk/app.h"

namespace cszb_scoreboard {
class ScreenPreview;
}  // namespace cszb_scoreboard

namespace cszb_scoreboard::test {

static char arg0[] = "scoreboard_testing.exe";
static char arg1[] = "-n";

std::array<char*, 3> TEST_ARGV = {{arg0, arg1, nullptr}};

void GuiTest::SetUp() {
  app = new Scoreboard();
  startApp(app);
  mainView()->updateWindow();
}

void GuiTest::TearDown() {
  Scoreboard::close();
  wxEntryCleanup();
}

void GuiTest::startApp(wxApp* app) {
  wxSetAssertHandler(nullptr);
  wxApp::SetInstance(app);
  // Argument to wxEntryStart cannot be const, so copy to a non-const before
  // calling.
  int argc = TEST_ARGV.size() - 1;
  wxEntryStart(argc, TEST_ARGV.data());
  app->OnInit();
}

auto GuiTest::mainView() -> MainView* {
  return dynamic_cast<MainView*>(
      Singleton::getInstance()->frameManager()->mainView());
}

auto GuiTest::textEntry() -> TextEntry* {
  return findController<TextEntry>(true);
}

auto GuiTest::firstPreview() -> ScreenPreview* {
  return mainView()->previewPanel()->preview(0);
}

ImageAnalysis::ImageAnalysis(Panel* panel, ImageAnalysisMode scan_mode) {
  wxRect dimensions = panel->wx()->GetRect();

  // Try to render offscreen if it's a ScreenTextSide
  auto* text_side = dynamic_cast<ScreenTextSide*>(panel);
  if (text_side != nullptr) {
    wxBitmap bitmap(dimensions.GetWidth(), dimensions.GetHeight());
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.SetBackground(*wxWHITE_BRUSH);
    memDC.Clear();

    auto renderer = RenderContext::forDC(&memDC);
    text_side->paintEvent(renderer.get());
    memDC.SelectObject(wxNullBitmap);

    Image rendered_image(bitmap);

    int total = 1;
    switch (scan_mode) {
      case IA_MODE_CENTERLINE_SCAN: {
        int y = dimensions.GetHeight() / 2;
        for (int x = 0; x < dimensions.GetWidth(); ++x) {
          wxColour color(rendered_image.GetRed(x, y),
                         rendered_image.GetGreen(x, y),
                         rendered_image.GetBlue(x, y));
          color_counts[color.GetRGB()]++;
        }
        total = dimensions.GetWidth();
        break;
      }
      case IA_MODE_QUARTER_SCAN: {
        int quarter_step =
            dimensions.GetHeight() / 4;  // NOLINT(readability-magic-numbers)
        if (quarter_step == 0) quarter_step = 1;
        int lines = 0;
        for (int y = 0; y < dimensions.GetHeight(); y += quarter_step) {
          lines++;
          for (int x = 0; x < dimensions.GetWidth(); ++x) {
            wxColour color(rendered_image.GetRed(x, y),
                           rendered_image.GetGreen(x, y),
                           rendered_image.GetBlue(x, y));
            color_counts[color.GetRGB()]++;
          }
        }
        total = dimensions.GetWidth() * lines;
        break;
      }
      default: {
        for (int x = 0; x < dimensions.GetWidth(); ++x) {
          for (int y = 0; y < dimensions.GetHeight(); ++y) {
            wxColour color(rendered_image.GetRed(x, y),
                           rendered_image.GetGreen(x, y),
                           rendered_image.GetBlue(x, y));
            color_counts[color.GetRGB()]++;
          }
        }
        total = dimensions.GetWidth() * dimensions.GetHeight();
        break;
      }
    }

    for (auto iterator : color_counts) {
      unsigned int color_rgb = iterator.first;
      float count = iterator.second;
      color_percentages[color_rgb] =
          count * 100 / total;  // NOLINT(readability-magic-numbers)
    }
    return;
  }

  wxWindow* widget = panel->wx();
  wxClientDC dc(widget);

  int total = 1;

  switch (scan_mode) {
    case IA_MODE_CENTERLINE_SCAN:
      countCenterlinePixels(dc, dimensions);
      total = dimensions.GetWidth();
      break;
    case IA_MODE_QUARTER_SCAN:
      countQuarterScanlinePixels(dc, dimensions);
      total = dimensions.GetWidth() * 5;  // NOLINT(readability-magic-numbers)
      break;
    default:
      countAllPixels(dc, dimensions);
      total = dimensions.GetWidth() * dimensions.GetHeight();
  }

  for (auto iterator : color_counts) {
    unsigned int color_rgb = iterator.first;
    float count = iterator.second;
    color_percentages[color_rgb] =
        count * 100 / total;  // NOLINT(readability-magic-numbers) 100 isn't
                              // that magic to calculate a percentage.
  }
}

void ImageAnalysis::countAllPixels(const wxClientDC& dc,
                                   const wxRect& dimensions) {
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

void ImageAnalysis::countQuarterScanlinePixels(const wxClientDC& dc,
                                               const wxRect& dimensions) {
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

void ImageAnalysis::countCenterlinePixels(const wxClientDC& dc,
                                          const wxRect& dimensions) {
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

auto ImageAnalysis::colorPercentage(const wxColour& color) const -> float {
  if (color_percentages.find(color.GetRGB()) == color_percentages.end()) {
    return 0;
  }
  return color_percentages.at(color.GetRGB());
}

auto ImageAnalysis::colorAmount(const wxColour& color) -> float {
  if (color_counts.find(color.GetRGB()) == color_counts.end()) {
    return 0;
  }
  return color_counts[color.GetRGB()];
}

auto ImageAnalysis::colorList() -> std::vector<int> {
  std::vector<int> color_list;
  color_list.reserve(color_counts.size());
  for (auto count : color_counts) {
    color_list.push_back(count.first);
  }
  return color_list;
}

}  // namespace cszb_scoreboard::test
