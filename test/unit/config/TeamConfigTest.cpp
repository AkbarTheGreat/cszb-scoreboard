/*
test/unit/config/TeamConfigTest.cpp: Tests for config/TeamConfig

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

#include "config/TeamConfig.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <wx/gdicmn.h>

#include "config/Persistence.h"
#include "test/mocks/config/MockPersistence.h"
#include "test/mocks/util/MockSingleton.h"
#include "ui/graphics/Color.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard::test {

using ::testing::Return;

class TeamConfigTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockPersistence> persist;

  void SetUp() override {
    if (wxTheColourDatabase == nullptr) {
      wxTheColourDatabase = new wxColourDatabase();
    }
    singleton = std::make_unique<MockSingleton>();
    persist = std::make_unique<MockPersistence>(singleton.get());
    EXPECT_CALL(*singleton, persistence()).WillRepeatedly(Return(persist.get()));
  }
};

TEST_F(TeamConfigTest, InitializesWithDefaultsWhenEmpty) {
  proto::TeamConfig empty;
  persist->saveTeams(empty);

  TeamConfig config(SingletonClass{}, singleton.get());

  // Should auto-initialize home and away teams
  EXPECT_EQ(config.numberOfTeams(), 2);
  EXPECT_EQ(config.teamColor(proto::TeamInfo_TeamType_HOME_TEAM), Color("Blue"));
  EXPECT_EQ(config.teamColor(proto::TeamInfo_TeamType_AWAY_TEAM), Color("Red"));

  std::vector<proto::TeamInfo_TeamType> order = config.singleScreenOrder();
  ASSERT_EQ(order.size(), 2);
  EXPECT_EQ(order[0], proto::TeamInfo_TeamType_HOME_TEAM);
  EXPECT_EQ(order[1], proto::TeamInfo_TeamType_AWAY_TEAM);
}

TEST_F(TeamConfigTest, TeamColorsAndOrder) {
  proto::TeamConfig initial;
  auto* home = initial.add_teams();
  home->set_team_type(proto::TeamInfo_TeamType_HOME_TEAM);
  ProtoUtil::protoClr(Color(0, 128, 0), home->mutable_team_color()); // Green Home

  auto* away = initial.add_teams();
  away->set_team_type(proto::TeamInfo_TeamType_AWAY_TEAM);
  ProtoUtil::protoClr(Color(255, 255, 0), away->mutable_team_color()); // Yellow Away

  persist->saveTeams(initial);

  TeamConfig config(SingletonClass{}, singleton.get());
  EXPECT_EQ(config.teamColor(proto::TeamInfo_TeamType_HOME_TEAM), Color("Green"));
  EXPECT_EQ(config.teamColor(proto::TeamInfo_TeamType_AWAY_TEAM), Color("Yellow"));

  // Modify color
  config.setColor(proto::TeamInfo_TeamType_HOME_TEAM, Color("Purple"));
  EXPECT_EQ(config.teamColor(proto::TeamInfo_TeamType_HOME_TEAM), Color("Purple"));

  // Change single screen order
  std::vector<proto::TeamInfo_TeamType> new_order = {
      proto::TeamInfo_TeamType_AWAY_TEAM, proto::TeamInfo_TeamType_HOME_TEAM};
  config.setSingleScreenOrder(new_order);
  EXPECT_EQ(config.singleScreenOrder(), new_order);

  // Save settings
  config.saveSettings();
  proto::TeamConfig saved = persist->loadTeams();
  EXPECT_EQ(saved.single_screen_order(0), proto::TeamInfo_TeamType_AWAY_TEAM);
}

TEST_F(TeamConfigTest, TeamColorForSide) {
  proto::TeamConfig initial;
  auto* home = initial.add_teams();
  home->set_team_type(proto::TeamInfo_TeamType_HOME_TEAM);
  ProtoUtil::protoClr(Color(255, 0, 0), home->mutable_team_color()); // Red Home

  auto* away = initial.add_teams();
  away->set_team_type(proto::TeamInfo_TeamType_AWAY_TEAM);
  ProtoUtil::protoClr(Color(0, 0, 255), away->mutable_team_color()); // Blue Away

  persist->saveTeams(initial);

  TeamConfig config(SingletonClass{}, singleton.get());

  proto::ScreenSide side;
  side.set_home(true);
  side.set_away(false);

  std::vector<Color> colors = config.teamColor(side);
  ASSERT_EQ(colors.size(), 1);
  EXPECT_EQ(colors[0], Color("Red"));

  side.set_away(true);
  colors = config.teamColor(side);
  ASSERT_EQ(colors.size(), 2);
  EXPECT_EQ(colors[1], Color("Blue"));
}

TEST_F(TeamConfigTest, StaticHelpers) {
  EXPECT_EQ(TeamConfig::teamName(proto::TeamInfo_TeamType_HOME_TEAM), "Home");
  EXPECT_EQ(TeamConfig::teamName(proto::TeamInfo_TeamType_AWAY_TEAM), "Away");
  EXPECT_EQ(TeamConfig::teamName(proto::TeamInfo_TeamType_EXTRA_TEAM), "Unknown");
}

}  // namespace cszb_scoreboard::test
