/*
test/ProtoUtilTest.cpp: Tests for util/ProtoUtil

Copyright 2019 Tracy Beck

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

#include "gtest/gtest.h"
#include "config.pb.h"
#include "util/ProtoUtil.h"

extern wxColourDatabase *wxTheColourDatabase;

namespace cszb_scoreboard {
namespace test {

class ProtoUtilTest : public testing::Test {
 protected:
  virtual void SetUp() override {
    // Wx doesn't initialize this database just by using wxColour, so we'll
    // initialize it here for testing if needed.
    if (!wxTheColourDatabase) wxTheColourDatabase = new wxColourDatabase();
  }
};

TEST_F(ProtoUtilTest, ConvertsARectangleCorrectly) {
  proto::Rectangle known;
  known.set_x(2);
  known.set_y(3);
  known.set_width(10);
  known.set_height(20);
  wxRect converted = ProtoUtil::wxRct(known);
  EXPECT_EQ(converted.GetX(), 2);
  EXPECT_EQ(converted.GetY(), 3);
  EXPECT_EQ(converted.GetWidth(), 10);
  EXPECT_EQ(converted.GetHeight(), 20);
}

TEST_F(ProtoUtilTest, ConvertsAWxRectCorrectly) {
  wxRect known(2, 3, 10, 20);
  proto::Rectangle converted;
  ProtoUtil::protoRct(known, &converted);
  EXPECT_EQ(converted.x(), 2);
  EXPECT_EQ(converted.y(), 3);
  EXPECT_EQ(converted.width(), 10);
  EXPECT_EQ(converted.height(), 20);
}

TEST_F(ProtoUtilTest, CovertsAWxColorCorrectly) {
  Color wx_white("White");
  unsigned int white_rgb = 0xFFFFFF;

  Color wx_red("Red");
  unsigned int red_rgb = 0x0000FF;

  Color wx_black("Black");
  unsigned int black_rgb = 0x000000;

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

TEST_F(ProtoUtilTest, CovertsAProtoColorCorrectly) {
  Color wx_white("White");
  proto::Color proto_white;
  proto_white.set_rgb(0xFFFFFF);

  Color wx_red("Red");
  proto::Color proto_red;
  proto_red.set_rgb(0x0000FF);

  Color wx_black("Black");
  proto::Color proto_black;
  proto_black.set_rgb(0x000000);

  Color converted_white = ProtoUtil::wxClr(proto_white);
  EXPECT_EQ(converted_white, wx_white);

  Color converted_red = ProtoUtil::wxClr(proto_red);
  EXPECT_EQ(converted_red, wx_red);

  Color converted_black = ProtoUtil::wxClr(proto_black);
  EXPECT_EQ(converted_black, wx_black);
}

}  // namespace test
}  // namespace cszb_scoreboard
