/*
test/unit/util/TimerManagerTest.cpp: Tests for util/TimerManager

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "test/mocks/util/MockSingleton.h"
#include "util/TimerManager.h"

namespace cszb_scoreboard::test {

using ::testing::Return;

TEST(TimerManagerTest, BasicStateAndFormatting) {
  MockSingleton singleton;
  TimerManager timer_manager(SingletonClass{}, &singleton);

  EXPECT_FALSE(timer_manager.timerOn());
  EXPECT_FALSE(timer_manager.timerRunning());

  // Default is 1 minute (60 seconds)
  EXPECT_EQ(timer_manager.displayTime(), "1:00");

  timer_manager.setTime(95);
  EXPECT_EQ(timer_manager.displayTime(), "1:35");

  timer_manager.setTime(5);
  EXPECT_EQ(timer_manager.displayTime(), "0:05");
}

TEST(TimerManagerTest, StartPauseTimer) {
  MockSingleton singleton;
  TimerManager timer_manager(SingletonClass{}, &singleton);

  timer_manager.setTime(10);
  timer_manager.startTimer();
  EXPECT_TRUE(timer_manager.timerRunning());

  // Start it again, should be a no-op
  timer_manager.startTimer();
  EXPECT_TRUE(timer_manager.timerRunning());

  timer_manager.pauseTimer();
  EXPECT_FALSE(timer_manager.timerRunning());
}

TEST(TimerManagerTest, ShowHideTimer) {
  MockSingleton singleton;
  TimerManager timer_manager(SingletonClass{}, &singleton);

  // showTimer should request the autoRefreshTimer from singleton
  EXPECT_CALL(singleton, autoRefreshTimer()).WillOnce(Return(nullptr));

  timer_manager.showTimer();
  EXPECT_TRUE(timer_manager.timerOn());

  // Second call shouldn't call autoRefreshTimer again since it's already
  // displayed
  timer_manager.showTimer();
  EXPECT_TRUE(timer_manager.timerOn());

  timer_manager.hideTimer();
  EXPECT_FALSE(timer_manager.timerOn());

  // Second hide is a no-op
  timer_manager.hideTimer();
  EXPECT_FALSE(timer_manager.timerOn());
}

}  // namespace cszb_scoreboard::test
