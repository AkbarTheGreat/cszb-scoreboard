/*
test/DisplayConfigTest.cpp: Tests for config/DisplayConfig

Copyright 2021-2023 Tracy Beck

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

#include <google/protobuf/util/message_differencer.h>  // for MessageDiffere...
#include <gtest/gtest.h>                               // IWYU pragma: keep for SuiteApiResolver
#include <wx/gdicmn.h>                                 // for wxRect, wxPoint

#include <cstdint>  // for int32_t
#include <memory>   // for unique_ptr
#include <string>   // for string

#include "config.pb.h"                             // for DisplayConfig
#include "config/DisplayConfig.h"                  // for DisplayConfig
#include "gmock/gmock.h"                           // for Return, Return...
#include "test/TestUtil.h"                         // for EXPECT_ASSERT
#include "test/mocks/config/MockPersistence.h"     // for MockPersistence
#include "test/mocks/ui/frame/MockFrameManager.h"  // for MockFrameManager
#include "test/mocks/ui/frame/MockMainView.h"      // for MockMainView
#include "test/mocks/ui/widget/swx/MockDisplay.h"  // for MockDisplay
#include "test/mocks/ui/widget/swx/MockFrame.h"    // for MockFrame
#include "test/mocks/util/MockSingleton.h"         // for MockSingleton
#include "ui/component/ScreenPresenter.h"          // for ScreenPresenter
#include "ui/frame/MainView.h"                     // for MainView
#include "ui/widget/Display.h"                     // for Display
#include "ui/widget/Frame.h"                       // for Frame
#include "util/Singleton.h"                        // for SingletonClass

namespace cszb_scoreboard {
class ScreenText;

namespace swx {
class Frame;
}  // namespace swx
}  // namespace cszb_scoreboard

// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

using ::testing::_;
using ::testing::Return;

#define EXPECT_PROTO_EQ(expected, actual)          \
  {                                                \
    std::string diff_report;                       \
    diff_report.reserve(1024);                     \
    google::protobuf::util::MessageDifferencer md; \
    md.set_report_matches(false);                  \
    md.set_report_moves(false);                    \
    md.ReportDifferencesToString(&diff_report);    \
    EXPECT_TRUE(md.Compare(expected, actual))      \
        << "proto diff:\n"                         \
        << diff_report << "end proto diff";        \
  }

namespace cszb_scoreboard {

// Defining some constructors here so as to avoid having to link all of the
// scoreboard for MockFrameManager

MainView::MainView(swx::Frame *wx, Singleton *singleton) : Frame(wx) {}

ScreenPresenter::ScreenPresenter(int monitor_number, const ScreenText &preview,
                                 Singleton *singleton)
    : Frame("Scoreboard") {}

namespace test {

constexpr int32_t EXPECTED_DEFAULT_WINDOW_WIDTH = 1024;
constexpr int32_t EXPECTED_DEFAULT_WINDOW_HEIGHT = 768;

class DisplayConfigTest : public ::testing::Test {
 protected:
  std::unique_ptr<proto::DisplayConfig> display_config;
  std::unique_ptr<MockFrameManager> frame_manager;
  std::unique_ptr<swx::MockFrame> main_view_frame;
  std::unique_ptr<MockMainView> main_view;
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockPersistence> persist;

  DisplayConfigTest() = default;
  ~DisplayConfigTest() override = default;

  void SetUp() override {
    display_config = defaultConfig();
    singleton = std::make_unique<MockSingleton>();
    persist = std::make_unique<MockPersistence>(singleton.get());
    frame_manager = std::make_unique<MockFrameManager>();
    main_view_frame = std::make_unique<swx::MockFrame>();
    main_view =
        std::make_unique<MockMainView>(main_view_frame.get(), singleton.get());
    EXPECT_CALL(*main_view, refreshPreviews).WillRepeatedly(Return());
    EXPECT_CALL(*main_view_frame, Destroy)
        .WillRepeatedly(
            Return(true));  // Uninteresting destruction in this test.
    EXPECT_CALL(*singleton, persistence).WillRepeatedly(Return(persist.get()));
    EXPECT_CALL(*singleton, frameManager)
        .WillRepeatedly(Return(frame_manager.get()));
    EXPECT_CALL(*frame_manager, mainView)
        .WillRepeatedly(Return(main_view.get()));
    EXPECT_CALL(*persist, saveDisplays).WillRepeatedly(Return());
    // Default the main_view to be in monitor1()
    EXPECT_CALL(*main_view_frame, GetPosition)
        .WillRepeatedly(Return(wxPoint(1, 1)));
    // Since the config might change, only return it once.  Futher returns in a
    // test should specify what they want to happen.
    EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  }

  void TearDown() override {
    display_config.reset();
    frame_manager.reset();
    singleton.reset();
    persist.reset();
  }

  static auto defaultConfig() -> std::unique_ptr<proto::DisplayConfig> {
    auto config = std::make_unique<proto::DisplayConfig>();
    config->set_enable_windowed_mode(true);
    config->set_window_count(2);
    proto::DisplayInfo *display_info = config->add_displays();
    // NOLINTNEXTLINE(readability-magic-numbers)
    displaySize(display_info, 5, 10, 1024, 768);
    display_info->set_id(0);
    display_info->mutable_side()->set_control(true);
    display_info->mutable_side()->set_home(true);
    display_info = config->add_displays();
    // NOLINTNEXTLINE(readability-magic-numbers)
    displaySize(display_info, 25, 30, 640, 480);
    display_info->set_id(1);
    display_info->mutable_side()->set_away(true);
    return config;
  }

  // Large primary monitor
  static auto monitor1() -> Display {
    auto wx = std::make_shared<swx::MockDisplay>();
    EXPECT_CALL(*wx, IsPrimary).WillRepeatedly(Return(true));
    EXPECT_CALL(*wx, GetGeometry)
        // NOLINTNEXTLINE(readability-magic-numbers)
        .WillRepeatedly(Return(wxRect(0, 0, 1024, 768)));
    return Display(wx);
  }

  // Large extended monitor
  static auto monitor2() -> Display {
    auto wx = std::make_shared<swx::MockDisplay>();
    EXPECT_CALL(*wx, IsPrimary).WillRepeatedly(Return(false));
    EXPECT_CALL(*wx, GetGeometry)
        // NOLINTNEXTLINE(readability-magic-numbers)
        .WillRepeatedly(Return(wxRect(1024, 768, 1024, 768)));
    return Display(wx);
  }

  // Small extended monitor
  static auto monitor3() -> Display {
    auto wx = std::make_shared<swx::MockDisplay>();
    EXPECT_CALL(*wx, IsPrimary).WillRepeatedly(Return(false));
    EXPECT_CALL(*wx, GetGeometry)
        // NOLINTNEXTLINE(readability-magic-numbers)
        .WillRepeatedly(Return(wxRect(2048, 1536, 640, 480)));
    return Display(wx);
  }

  static void displaySize(proto::DisplayInfo *display_info, int32_t x,
                          int32_t y, int32_t width, int32_t height) {
    display_info->mutable_dimensions()->set_x(x);
    display_info->mutable_dimensions()->set_y(y);
    display_info->mutable_dimensions()->set_width(width);
    display_info->mutable_dimensions()->set_height(height);
  }
};

TEST_F(DisplayConfigTest, NumberOfDisplays) {
  // A two window setup returns the right count.
  DisplayConfig dualConfig(SingletonClass{}, singleton.get());
  EXPECT_EQ(2, dualConfig.numberOfDisplays());

  // Number increases with a fully set up display from disk
  display_config->add_displays();
  display_config->set_window_count(3);
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  DisplayConfig triConfig(SingletonClass{}, singleton.get());
  EXPECT_EQ(3, triConfig.numberOfDisplays());

  // For Windowed mode, the only thing that really matters is window count.
  display_config.reset();
  display_config = defaultConfig();
  display_config->set_window_count(3);
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  DisplayConfig autoConfig(SingletonClass{}, singleton.get());
  EXPECT_EQ(3, autoConfig.numberOfDisplays());
}

TEST_F(DisplayConfigTest, ExternalConfigSkipsIfCountIsIdentical) {
  // Load display to flush the call expected in SetUp.
  persist->loadDisplays();
  display_config->set_enable_windowed_mode(false);
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  EXPECT_CALL(*frame_manager, monitorCount).WillOnce(Return(2));
  EXPECT_CALL(*frame_manager, monitor(0)).WillRepeatedly(Return(monitor1()));
  EXPECT_CALL(*frame_manager, monitor(1)).WillRepeatedly(Return(monitor2()));
  DisplayConfig config(SingletonClass{}, singleton.get());

  // Display 1 would change as well as some other details if it was actually
  // re-calculating after removing windowed_mode.
  std::unique_ptr<proto::DisplayConfig> expected = defaultConfig();
  expected->set_enable_windowed_mode(false);
  expected->mutable_window_size()->set_width(EXPECTED_DEFAULT_WINDOW_WIDTH);
  expected->mutable_window_size()->set_height(EXPECTED_DEFAULT_WINDOW_HEIGHT);

  EXPECT_PROTO_EQ(*expected, config.displayConfig());
}

TEST_F(DisplayConfigTest, SingleExternalMonitorSetup) {
  // Load display to flush the call expected in SetUp.
  persist->loadDisplays();
  display_config.reset();
  display_config = std::make_unique<proto::DisplayConfig>();
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  EXPECT_CALL(*frame_manager, monitorCount).WillOnce(Return(1));
  EXPECT_CALL(*frame_manager, monitor(0)).WillRepeatedly(Return(monitor1()));
  DisplayConfig config(SingletonClass{}, singleton.get());

  proto::DisplayConfig expected;
  expected.mutable_window_size()->set_width(EXPECTED_DEFAULT_WINDOW_WIDTH);
  expected.mutable_window_size()->set_height(EXPECTED_DEFAULT_WINDOW_HEIGHT);
  proto::DisplayInfo *expected_display = expected.add_displays();
  // NOLINTNEXTLINE(readability-magic-numbers)
  displaySize(expected_display, 0, 0, 1024, 768);
  expected_display->mutable_side()->set_control(true);
  expected_display->mutable_side()->set_error(true);

  EXPECT_PROTO_EQ(expected, config.displayConfig());
}

TEST_F(DisplayConfigTest, DualExternalMonitorSetup) {
  // Load display to flush the call expected in SetUp.
  persist->loadDisplays();
  display_config.reset();
  display_config = std::make_unique<proto::DisplayConfig>();
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  EXPECT_CALL(*frame_manager, monitorCount).WillOnce(Return(2));
  EXPECT_CALL(*frame_manager, monitor(0)).WillRepeatedly(Return(monitor1()));
  EXPECT_CALL(*frame_manager, monitor(1)).WillRepeatedly(Return(monitor2()));
  DisplayConfig config(SingletonClass{}, singleton.get());

  proto::DisplayConfig expected;
  expected.mutable_window_size()->set_width(EXPECTED_DEFAULT_WINDOW_WIDTH);
  expected.mutable_window_size()->set_height(EXPECTED_DEFAULT_WINDOW_HEIGHT);
  proto::DisplayInfo *expected_display = expected.add_displays();
  // NOLINTNEXTLINE(readability-magic-numbers)
  displaySize(expected_display, 0, 0, 1024, 768);
  expected_display->mutable_side()->set_control(true);
  expected_display = expected.add_displays();
  expected_display->set_id(1);
  // NOLINTNEXTLINE(readability-magic-numbers)
  displaySize(expected_display, 1024, 768, 1024, 768);
  expected_display->mutable_side()->set_home(true);

  EXPECT_PROTO_EQ(expected, config.displayConfig());
}

TEST_F(DisplayConfigTest, TripleExternalMonitorSetup) {
  // Load display to flush the call expected in SetUp.
  persist->loadDisplays();
  display_config.reset();
  display_config = std::make_unique<proto::DisplayConfig>();
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  EXPECT_CALL(*frame_manager, monitorCount).WillOnce(Return(3));
  EXPECT_CALL(*frame_manager, monitor(0)).WillRepeatedly(Return(monitor1()));
  EXPECT_CALL(*frame_manager, monitor(1)).WillRepeatedly(Return(monitor2()));
  EXPECT_CALL(*frame_manager, monitor(2)).WillRepeatedly(Return(monitor3()));
  DisplayConfig config(SingletonClass{}, singleton.get());

  proto::DisplayConfig expected;
  expected.mutable_window_size()->set_width(EXPECTED_DEFAULT_WINDOW_WIDTH);
  expected.mutable_window_size()->set_height(EXPECTED_DEFAULT_WINDOW_HEIGHT);
  proto::DisplayInfo *expected_display = expected.add_displays();
  // NOLINTNEXTLINE(readability-magic-numbers)
  displaySize(expected_display, 0, 0, 1024, 768);
  expected_display->mutable_side()->set_control(true);
  expected_display = expected.add_displays();
  expected_display->set_id(1);
  // NOLINTNEXTLINE(readability-magic-numbers)
  displaySize(expected_display, 1024, 768, 1024, 768);
  expected_display->mutable_side()->set_home(true);
  expected_display = expected.add_displays();
  expected_display->set_id(2);
  // NOLINTNEXTLINE(readability-magic-numbers)
  displaySize(expected_display, 2048, 1536, 640, 480);
  expected_display->mutable_side()->set_away(true);

  EXPECT_PROTO_EQ(expected, config.displayConfig());
}

TEST_F(DisplayConfigTest, SetSide) {
  DisplayConfig config(SingletonClass{}, singleton.get());
  auto *newSide = new proto::ScreenSide();
  newSide->set_away(false);
  newSide->set_extra(true);
  config.setSide(1, *newSide);
  std::unique_ptr<proto::DisplayConfig> expected = defaultConfig();
  expected->mutable_window_size()->set_width(EXPECTED_DEFAULT_WINDOW_WIDTH);
  expected->mutable_window_size()->set_height(EXPECTED_DEFAULT_WINDOW_HEIGHT);
  expected->mutable_displays(1)->set_allocated_side(newSide);
  EXPECT_PROTO_EQ(*expected, config.displayConfig());
}

TEST_F(DisplayConfigTest, SetDisplayId) {
  DisplayConfig config(SingletonClass{}, singleton.get());
  EXPECT_TRUE(
      config.setDisplayId(1, 15));  // NOLINT (readability-magic-numbers)
  std::unique_ptr<proto::DisplayConfig> expected = defaultConfig();
  expected->mutable_window_size()->set_width(EXPECTED_DEFAULT_WINDOW_WIDTH);
  expected->mutable_window_size()->set_height(EXPECTED_DEFAULT_WINDOW_HEIGHT);
  expected->mutable_displays(1)->set_id(
      15);  // NOLINT (readability-magic-numbers)
  EXPECT_PROTO_EQ(*expected, config.displayConfig());

  // Setting the same display a second time should return false and change
  // nothing.
  EXPECT_FALSE(
      config.setDisplayId(1, 15));  // NOLINT (readability-magic-numbers)
  EXPECT_PROTO_EQ(*expected, config.displayConfig());

  EXPECT_ASSERT(config.setDisplayId(-1, 1));
  EXPECT_ASSERT(config.setDisplayId(3, 1));
}

}  // namespace test
}  // namespace cszb_scoreboard
