/*
test/DisplayConfigTest.cpp: Tests for config/DisplayConfig

Copyright 2021 Tracy Beck

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

#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>  // IWYU pragma: keep

#include "config.pb.h"
#include "config/DisplayConfig.h"
#include "test/mocks/config/MockPersistence.h"
#include "test/mocks/ui/frame/MockFrameManager.h"
#include "test/mocks/ui/frame/MockMainView.h"
#include "test/mocks/ui/widget/swx/MockDisplay.h"
#include "test/mocks/util/MockSingleton.h"
#include "ui/component/ScreenPresenter.h"
#include "ui/widget/Display.h"

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

 private:
  static void displaySize(proto::DisplayInfo *display_info, int32_t x,
                          int32_t y, int32_t width, int32_t height) {
    display_info->mutable_dimensions()->set_x(x);
    display_info->mutable_dimensions()->set_y(y);
    display_info->mutable_dimensions()->set_width(width);
    display_info->mutable_dimensions()->set_height(height);
  }
};

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
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

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(DisplayConfigTest, SingleExternalMonitorSetup) {
  persist->loadDisplays();
  display_config.reset();
  display_config = std::make_unique<proto::DisplayConfig>();
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  EXPECT_CALL(*frame_manager, monitorCount).WillOnce(Return(1));
  EXPECT_CALL(*frame_manager, monitor(0)).WillRepeatedly(Return(monitor1()));
  DisplayConfig config(SingletonClass{}, singleton.get());

  proto::DisplayConfig expected;
  expected.mutable_window_size()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected.mutable_window_size()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  proto::DisplayInfo *expected_display = expected.add_displays();
  expected_display->mutable_dimensions()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_side()->set_control(true);
  expected_display->mutable_side()->set_error(true);

  EXPECT_PROTO_EQ(expected, config.displayConfig());
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(DisplayConfigTest, DualExternalMonitorSetup) {
  persist->loadDisplays();
  display_config.reset();
  display_config = std::make_unique<proto::DisplayConfig>();
  EXPECT_CALL(*persist, loadDisplays).WillOnce(Return(*display_config));
  EXPECT_CALL(*frame_manager, monitorCount).WillOnce(Return(2));
  EXPECT_CALL(*frame_manager, monitor(0)).WillRepeatedly(Return(monitor1()));
  EXPECT_CALL(*frame_manager, monitor(1)).WillRepeatedly(Return(monitor2()));
  DisplayConfig config(SingletonClass{}, singleton.get());

  proto::DisplayConfig expected;
  expected.mutable_window_size()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected.mutable_window_size()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  proto::DisplayInfo *expected_display = expected.add_displays();
  expected_display->mutable_dimensions()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_side()->set_control(true);
  expected_display = expected.add_displays();
  expected_display->set_id(1);
  expected_display->mutable_dimensions()->set_x(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_y(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_side()->set_home(true);

  EXPECT_PROTO_EQ(expected, config.displayConfig());
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(DisplayConfigTest, TripleExternalMonitorSetup) {
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
  expected.mutable_window_size()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected.mutable_window_size()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  proto::DisplayInfo *expected_display = expected.add_displays();
  expected_display->mutable_dimensions()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_side()->set_control(true);
  expected_display = expected.add_displays();
  expected_display->set_id(1);
  expected_display->mutable_dimensions()->set_x(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_y(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_width(
      1024);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_height(
      768);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_side()->set_home(true);
  expected_display = expected.add_displays();
  expected_display->set_id(2);
  expected_display->mutable_dimensions()->set_x(
      2048);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_y(
      1536);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_width(
      640);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_dimensions()->set_height(
      480);  // NOLINT(readability-magic-numbers)
  expected_display->mutable_side()->set_away(true);

  EXPECT_PROTO_EQ(expected, config.displayConfig());
}

}  // namespace test
}  // namespace cszb_scoreboard
