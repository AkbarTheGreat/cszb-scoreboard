/*
test/integration/ThingsModeTest.cpp: Tests for ui/component/control/ThingsMode

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

#include "ui/component/control/ThingsMode.h"

#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFa...

#include "gtest/gtest_pred_impl.h"                      // for TEST_F
#include "test/integration/GuiTest.h"                    // for WX_A, GuiTest
#include "ui/component/ControlPanel.h"                  // for ControlPanel
#include "ui/component/control/things_mode/ActivityPanel.h"
#include "ui/frame/MainView.h"                          // for MainView

namespace cszb_scoreboard::test {

class ThingsModeTest : public GuiTest {
 protected:
  static auto thingsMode() -> ThingsMode* {
    int i = 0;
    while (true) {
      auto* controller = mainView()->controlPanel()->textController(i);
      if (controller == nullptr) {
        break;
      }
      auto* things = dynamic_cast<ThingsMode*>(controller);
      if (things != nullptr) {
        return things;
      }
      i++;
    }
    return nullptr;
  }
};

TEST_F(ThingsModeTest, ActivityListManagement) {
  ThingsMode* things = thingsMode();
  ASSERT_NE(things, nullptr);

  // Default Game Mode is Single Team (index 0)
  // Verify home, away, and all activity panels exist
  ASSERT_NE(things->homePanel(), nullptr);
  ASSERT_NE(things->awayPanel(), nullptr);
  ASSERT_NE(things->allPanel(), nullptr);

  int initial_all_count = things->allPanel()->activitiesCount();
  int initial_home_count = things->homePanel()->activitiesCount();
  int initial_away_count = things->awayPanel()->activitiesCount();

  // 1. Select "All" screen and add an activity
  WX_A(things->selectScreen(2)); // Index 2 is All
  WX_A(things->clickNewActivity());
  EXPECT_EQ(things->allPanel()->activitiesCount(), initial_all_count + 1);

  // 2. Select "Home" screen and add an activity
  WX_A(things->selectScreen(0)); // Index 0 is Home
  WX_A(things->clickNewActivity());
  EXPECT_EQ(things->homePanel()->activitiesCount(), initial_home_count + 1);

  // 3. Select "Away" screen and add an activity
  WX_A(things->selectScreen(1)); // Index 1 is Away
  WX_A(things->clickNewActivity());
  EXPECT_EQ(things->awayPanel()->activitiesCount(), initial_away_count + 1);

  // 4. Test Head-to-Head Mode (index 1)
  WX_A(things->selectGameMode(1)); // Index 1 is Head-to-Head
  ASSERT_NE(things->headToHeadPanel(), nullptr);

  int initial_h2h_count = things->headToHeadPanel()->activitiesCount();
  WX_A(things->clickNewActivity());
  EXPECT_EQ(things->headToHeadPanel()->activitiesCount(), initial_h2h_count + 1);
}

}  // namespace cszb_scoreboard::test
