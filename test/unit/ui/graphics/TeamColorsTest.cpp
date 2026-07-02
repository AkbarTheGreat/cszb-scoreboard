/*
test/unit/ui/graphics/TeamColorsTest.cpp: Tests for ui/graphics/TeamColors

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

#include <wx/gdicmn.h>  // for wxTheColourDatabase

#include <memory>  // for unique_ptr, allocator

#include "config.pb.h"                          // for TeamInfo, TeamConfig
#include "config/TeamConfig.h"                  // for TeamConfig
#include "gmock/gmock.h"                        // for Return, ReturnAction
#include "gtest/gtest.h"                        // for EXPECT_EQ, Message
#include "test/mocks/config/MockPersistence.h"  // for MockPersistence
#include "test/mocks/util/MockSingleton.h"      // for MockSingleton
#include "ui/graphics/Color.h"                  // for Color
#include "ui/graphics/TeamColors.h"             // for TeamColors
#include "util/ProtoUtil.h"                     // for ProtoUtil
#include "util/Singleton.h"                     // for SingletonClass
// IWYU pragma: no_include <gtest/gtest.h>

namespace cszb_scoreboard::test {

using ::testing::Return;

class TeamColorsTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockPersistence> persistence;
  std::unique_ptr<TeamConfig> team_config;

  void SetUp() override {
    if (wxTheColourDatabase == nullptr) {
      wxTheColourDatabase = new wxColourDatabase();
    }
    singleton = std::make_unique<MockSingleton>();
    persistence = std::make_unique<MockPersistence>(singleton.get());
    EXPECT_CALL(*singleton, persistence())
        .WillRepeatedly(Return(persistence.get()));

    // Setup basic team configuration mock responses
    proto::TeamConfig default_config;
    auto* home = default_config.add_teams();
    home->set_team_type(proto::TeamInfo_TeamType_HOME_TEAM);
    ProtoUtil::protoClr(Color("Blue"), home->mutable_team_color());

    auto* away = default_config.add_teams();
    away->set_team_type(proto::TeamInfo_TeamType_AWAY_TEAM);
    ProtoUtil::protoClr(Color("Red"), away->mutable_team_color());

    persistence->saveTeams(default_config);

    team_config =
        std::make_unique<TeamConfig>(SingletonClass{}, singleton.get());
    EXPECT_CALL(*singleton, teamConfig())
        .WillRepeatedly(Return(team_config.get()));
  }
};

TEST_F(TeamColorsTest, DefaultColorsLoaded) {
  TeamColors colors(SingletonClass{}, singleton.get());

  EXPECT_EQ(colors.getColor(ProtoUtil::homeSide()), Color("Blue"));
  EXPECT_EQ(colors.getColor(ProtoUtil::awaySide()), Color("Red"));
  EXPECT_EQ(colors.getColor(ProtoUtil::allSide()), Color("Black"));
}

TEST_F(TeamColorsTest, ColorUpdatesAreSet) {
  TeamColors colors(SingletonClass{}, singleton.get());

  colors.setColor(ProtoUtil::homeSide(), Color("Green"));
  colors.setColor(ProtoUtil::awaySide(), Color("Yellow"));
  colors.setColor(ProtoUtil::allSide(), Color("White"));

  EXPECT_EQ(colors.getColor(ProtoUtil::homeSide()), Color("Green"));
  EXPECT_EQ(colors.getColor(ProtoUtil::awaySide()), Color("Yellow"));
  EXPECT_EQ(colors.getColor(ProtoUtil::allSide()), Color("White"));
}

}  // namespace cszb_scoreboard::test
