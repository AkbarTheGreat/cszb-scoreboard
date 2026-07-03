/*
test/integration/ScoreControlTest.cpp: Tests for
ui/component/control/ScoreControl

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

#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFa...

#include "gtest/gtest_pred_impl.h"      // for TEST_F
#include "test/integration/GuiTest.h"   // for WX_A, GuiTest
#include "ui/component/ControlPanel.h"  // for ControlPanel
#include "ui/component/control/ScoreControl.h"
#include "ui/frame/MainView.h"  // for MainView

namespace cszb_scoreboard::test {

class ScoreControlTest : public GuiTest {
 protected:
  static auto scoreControl() -> ScoreControl* {
    return findController<ScoreControl>();
  }
};

TEST_F(ScoreControlTest, ScoreAdjustments) {
  ScoreControl* score = scoreControl();
  ASSERT_NE(score, nullptr);

  // Set initial names
  WX_A(score->setHomeName("Home Team"));
  WX_A(score->setAwayName("Away Team"));
  EXPECT_EQ(score->homeName(), "Home Team");
  EXPECT_EQ(score->awayName(), "Away Team");

  // Set initial scores
  WX_A(score->setHomeScore("0"));
  WX_A(score->setAwayScore("0"));
  EXPECT_EQ(score->homeScore(), "0");
  EXPECT_EQ(score->awayScore(), "0");

  // Test home score additions
  WX_A(score->clickHomeAddOne());
  EXPECT_EQ(score->homeScore(), "1");

  WX_A(score->clickHomeAddFive());
  EXPECT_EQ(score->homeScore(), "6");

  WX_A(score->clickHomeMinusOne());
  EXPECT_EQ(score->homeScore(), "5");

  // Test away score additions
  WX_A(score->clickAwayAddOne());
  EXPECT_EQ(score->awayScore(), "1");

  WX_A(score->clickAwayAddFive());
  EXPECT_EQ(score->awayScore(), "6");

  WX_A(score->clickAwayMinusOne());
  EXPECT_EQ(score->awayScore(), "5");
}

TEST_F(ScoreControlTest, IntroToggling) {
  ScoreControl* score = scoreControl();
  ASSERT_NE(score, nullptr);

  // Default mode is match play (intro mode is false)
  EXPECT_FALSE(score->isIntro());

  // Toggle to Intro Mode
  WX_A(score->clickToggleIntro());
  EXPECT_TRUE(score->isIntro());

  // Toggle back to Match Play
  WX_A(score->clickToggleIntro());
  EXPECT_FALSE(score->isIntro());
}

}  // namespace cszb_scoreboard::test
