/*
test/unit/config/SlideShowTest.cpp: Tests for config/SlideShow

Copyright 2026 Tracy Beck

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

#include <wx/log.h>  // for wxLogNull

#include <chrono>  // IWYU pragma: keep for seconds
#include <memory>  // for allocator, unique_ptr
#include <thread>  // for sleep_for
#include <vector>  // for vector

#include "config/SlideShow.h"                   // for SlideShow
#include "config/swx/image.h"                   // for Image
#include "gmock/gmock.h"                        // for Return, ReturnAction
#include "gtest/gtest.h"                        // for AssertionResult, Message
#include "slide_show.pb.h"                      // for SlideInfo, SlideShow
#include "test/mocks/config/MockPersistence.h"  // for MockPersistence
#include "test/mocks/util/MockSingleton.h"      // for MockSingleton
#include "util/FilesystemPath.h"                // for FilesystemPath
#include "util/Singleton.h"                     // for SingletonClass, Singl...
// IWYU pragma: no_include <bits/chrono.h>
// IWYU pragma: no_include <gtest/gtest.h>

#define TEST_STUB_SINGLETON
#include "test/mocks/Stubs.h"  // for Singleton::getInstance

namespace cszb_scoreboard::test {

using ::testing::Return;

class SlideShowTest : public ::testing::Test {
 protected:
  MockSingleton* mock_singleton;
  std::unique_ptr<MockPersistence> mock_persistence;
  std::unique_ptr<wxLogNull> log_no;

  void SetUp() override {
    log_no = std::make_unique<wxLogNull>();
    mock_singleton = static_cast<MockSingleton*>(Singleton::getInstance());
    mock_persistence = std::make_unique<MockPersistence>(mock_singleton);
    EXPECT_CALL(*mock_singleton, persistence())
        .WillRepeatedly(Return(mock_persistence.get()));
  }

  void TearDown() override {
    mock_persistence.reset();
    log_no.reset();
  }
};

TEST_F(SlideShowTest, SlideOperations) {
  // Save an initial show with some slides
  proto::SlideShow initial_show;
  initial_show.set_delay(6.0);
  auto* slide1 = initial_show.add_slides();
  slide1->set_name("SlideA");
  slide1->set_file_path("fileA.png");
  auto* slide2 = initial_show.add_slides();
  slide2->set_name("SlideB");
  slide2->set_file_path("fileB.png");

  mock_persistence->saveSlideShow(initial_show);

  SlideShow show(SingletonClass{}, mock_singleton);

  EXPECT_EQ(show.delay(), 6.0);
  EXPECT_FALSE(show.isRunning());

  // Test slides retrieval
  auto list = show.slides(0, 2);
  ASSERT_EQ(list.size(), 2);
  EXPECT_EQ(list[0].name(), "SlideA");
  EXPECT_EQ(list[1].name(), "SlideB");

  // Test swap
  show.swapSlides(0, 1);
  list = show.slides(0, 2);
  EXPECT_EQ(list[0].name(), "SlideB");
  EXPECT_EQ(list[1].name(), "SlideA");

  // Invalid swaps should be ignored
  show.swapSlides(-1, 0);
  show.swapSlides(0, 5);
  list = show.slides(0, 2);
  EXPECT_EQ(list[0].name(), "SlideB");
  EXPECT_EQ(list[1].name(), "SlideA");

  // Test addSlide at end
  show.addSlide("SlideC", FilesystemPath("fileC.png"));
  list = show.slides(0, 3);
  ASSERT_EQ(list.size(), 3);
  EXPECT_EQ(list[2].name(), "SlideC");

  // Test addSlide with insertion index
  show.addSlide("SlideD", FilesystemPath("fileD.png"), 1);
  list = show.slides(0, 4);
  ASSERT_EQ(list.size(), 4);
  EXPECT_EQ(list[1].name(), "SlideD");  // Inserted at 1

  // Test removeSlide
  show.removeSlide(1);  // removes SlideD
  list = show.slides(0, 4);
  ASSERT_EQ(list.size(), 3);
  EXPECT_EQ(list[0].name(), "SlideB");
  EXPECT_EQ(list[1].name(), "SlideA");
  EXPECT_EQ(list[2].name(), "SlideC");

  // Test saveShow
  show.setDelay(8.0);
  show.saveShow();
  proto::SlideShow saved = mock_persistence->loadSlideShow();
  EXPECT_EQ(saved.delay(), 8.0);
  ASSERT_EQ(saved.slides_size(), 3);
  EXPECT_EQ(saved.slides(0).name(), "SlideB");
}

TEST_F(SlideShowTest, TimingAndTransitions) {
  proto::SlideShow initial_show;
  initial_show.set_delay(0.001);  // minimal delay for fast testing
  auto* slide1 = initial_show.add_slides();
  slide1->set_name("SlideA");
  slide1->set_file_path("");

  auto* slide2 = initial_show.add_slides();
  slide2->set_name("SlideB");
  slide2->set_file_path("");

  mock_persistence->saveSlideShow(initial_show);

  SlideShow show(SingletonClass{}, mock_singleton);
  show.start();
  EXPECT_TRUE(show.isRunning());

  // First call to nextSlide
  Image img1 = show.nextSlide();

  // Sleep slightly to ensure transition triggers
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  // Second call should transition to the next slide
  Image img2 = show.nextSlide();

  show.stop();
  EXPECT_FALSE(show.isRunning());
}

TEST_F(SlideShowTest, EmptyShowGracefulExit) {
  proto::SlideShow empty_show;
  mock_persistence->saveSlideShow(empty_show);

  SlideShow show(SingletonClass{}, mock_singleton);
  show.start();
  EXPECT_TRUE(show.isRunning());

  Image img = show.nextSlide();
  EXPECT_FALSE(show.isRunning());  // Stops running automatically if empty
}

}  // namespace cszb_scoreboard::test
