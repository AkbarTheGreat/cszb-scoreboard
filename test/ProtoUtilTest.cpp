/*
test/ProtoUtilTest.cpp: Tests for util/ProtoUtil

Copyright 2019-2021 Tracy Beck

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

#include "config.pb.h"
#include "gtest/gtest.h"
#include "util/ProtoUtil.h"

// extern wxColourDatabase *wxTheColourDatabase;

namespace cszb_scoreboard::test {

class ProtoUtilTest : public testing::Test {
 protected:
  void SetUp() override {
    // Wx doesn't initialize this database just by using wxColour, so we'll
    // initialize it here for testing if needed.
    if (wxTheColourDatabase == nullptr) {
      wxTheColourDatabase = new wxColourDatabase();
    }
  }
};

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(ProtoUtilTest, ConvertsARectangleCorrectly) {
  proto::Rectangle known;
  known.set_x(2);
  known.set_y(3);
  known.set_width(10);   // NOLINT(readability-magic-numbers)
  known.set_height(20);  // NOLINT(readability-magic-numbers)
  wxRect converted = ProtoUtil::wxRct(known);
  EXPECT_EQ(converted.GetX(), 2);
  EXPECT_EQ(converted.GetY(), 3);
  EXPECT_EQ(converted.GetWidth(), 10);   // NOLINT(readability-magic-numbers)
  EXPECT_EQ(converted.GetHeight(), 20);  // NOLINT(readability-magic-numbers)
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(ProtoUtilTest, ConvertsAWxRectCorrectly) {
  wxRect known(2, 3, 10, 20);  // NOLINT(readability-magic-numbers)
  proto::Rectangle converted;
  ProtoUtil::protoRct(known, &converted);
  EXPECT_EQ(converted.x(), 2);
  EXPECT_EQ(converted.y(), 3);
  EXPECT_EQ(converted.width(), 10);   // NOLINT(readability-magic-numbers)
  EXPECT_EQ(converted.height(), 20);  // NOLINT(readability-magic-numbers)
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(ProtoUtilTest, CovertsAWxColorCorrectly) {
  Color wx_white("White");
  unsigned int white_rgb = 0xFFFFFF;  // NOLINT(readability-magic-numbers)

  Color wx_red("Red");
  unsigned int red_rgb = 0x0000FF;  // NOLINT(readability-magic-numbers)

  Color wx_black("Black");
  unsigned int black_rgb = 0x000000;  // NOLINT(readability-magic-numbers)

  proto::Color converted_white;
  ProtoUtil::protoClr(wx_white, &converted_white);
  EXPECT_EQ(converted_white.rgb(), white_rgb);

  proto::Color converted_red;
  ProtoUtil::protoClr(wx_red, &converted_red);
  EXPECT_EQ(converted_red.rgb(), red_rgb);

  proto::Color converted_black;
  ProtoUtil::protoClr(wx_black, &converted_black);
  EXPECT_EQ(converted_black.rgb(), black_rgb);
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(ProtoUtilTest, CovertsAProtoColorCorrectly) {
  Color wx_white("White");
  proto::Color proto_white;
  proto_white.set_rgb(0xFFFFFF);  // NOLINT(readability-magic-numbers)

  Color wx_red("Red");
  proto::Color proto_red;
  proto_red.set_rgb(0x0000FF);  // NOLINT(readability-magic-numbers)

  Color wx_black("Black");
  proto::Color proto_black;
  proto_black.set_rgb(0x000000);  // NOLINT(readability-magic-numbers)

  Color converted_white = ProtoUtil::wxClr(proto_white);
  EXPECT_EQ(converted_white, wx_white);

  Color converted_red = ProtoUtil::wxClr(proto_red);
  EXPECT_EQ(converted_red, wx_red);

  Color converted_black = ProtoUtil::wxClr(proto_black);
  EXPECT_EQ(converted_black, wx_black);
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(ProtoUtilTest, SideContainsTests) {
  // Test all sides
  EXPECT_TRUE(ProtoUtil::sideContains(ProtoUtil::allSide(),
                                      proto::TeamInfo_TeamType_HOME_TEAM));
  EXPECT_TRUE(ProtoUtil::sideContains(ProtoUtil::allSide(),
                                      proto::TeamInfo_TeamType_AWAY_TEAM));
  EXPECT_TRUE(ProtoUtil::sideContains(ProtoUtil::allSide(),
                                      proto::TeamInfo_TeamType_EXTRA_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::allSide(),
                                       proto::TeamInfo_TeamType_TEAM_ERROR));

  // Test home side
  EXPECT_TRUE(ProtoUtil::sideContains(ProtoUtil::homeSide(),
                                      proto::TeamInfo_TeamType_HOME_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::homeSide(),
                                       proto::TeamInfo_TeamType_AWAY_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::homeSide(),
                                       proto::TeamInfo_TeamType_EXTRA_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::homeSide(),
                                       proto::TeamInfo_TeamType_TEAM_ERROR));

  // Test away side
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::awaySide(),
                                       proto::TeamInfo_TeamType_HOME_TEAM));
  EXPECT_TRUE(ProtoUtil::sideContains(ProtoUtil::awaySide(),
                                      proto::TeamInfo_TeamType_AWAY_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::awaySide(),
                                       proto::TeamInfo_TeamType_EXTRA_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::awaySide(),
                                       proto::TeamInfo_TeamType_TEAM_ERROR));

  // Test "no" side
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::noSide(),
                                       proto::TeamInfo_TeamType_HOME_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::noSide(),
                                       proto::TeamInfo_TeamType_AWAY_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::noSide(),
                                       proto::TeamInfo_TeamType_EXTRA_TEAM));
  EXPECT_FALSE(ProtoUtil::sideContains(ProtoUtil::noSide(),
                                       proto::TeamInfo_TeamType_TEAM_ERROR));
}

}  // namespace cszb_scoreboard::test
