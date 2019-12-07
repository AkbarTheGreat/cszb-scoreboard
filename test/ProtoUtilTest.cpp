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
#include "proto/config.pb.h"
#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

TEST(ProtoUtilTest, ConvertsARectangleCorrectly) {
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

TEST(ProtoUtilTest, ConvertsAWxRectCorrectly) {
  wxRect known(2, 3, 10, 20);
  proto::Rectangle converted;
  ProtoUtil::protoRct(known, &converted);
  EXPECT_EQ(converted.x(), 2);
  EXPECT_EQ(converted.y(), 3);
  EXPECT_EQ(converted.width(), 10);
  EXPECT_EQ(converted.height(), 20);
}

}  // namespace cszb_scoreboard
