/*
test/unit/config/PositionTest.cpp: Tests for config/Position

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

#include <wx/gdicmn.h>  // for wxPoint, wxSize

#include <memory>  // for allocator

#include "config.pb.h"        // for Rectangle
#include "config/Position.h"  // for Size, Position
#include "gtest/gtest.h"      // for AssertionResult, Message, TestPartResult
// IWYU pragma: no_include <gtest/gtest.h>

namespace cszb_scoreboard::test {

TEST(PositionTest, Conversions) {
  Position pos{10, 20};
  wxPoint wx_pt = pos.toWx();
  EXPECT_EQ(wx_pt.x, 10);
  EXPECT_EQ(wx_pt.y, 20);

  Position pos2 = Position::fromWx(wxPoint(30, 40));
  EXPECT_EQ(pos2.x, 30);
  EXPECT_EQ(pos2.y, 40);
}

TEST(PositionTest, Operators) {
  Position p1{10, 20};
  Position p2{5, 4};

  EXPECT_TRUE(p1 == p1);
  EXPECT_FALSE(p1 == p2);
  EXPECT_TRUE(p1 != p2);

  Position neg = -p1;
  EXPECT_EQ(neg.x, -10);
  EXPECT_EQ(neg.y, -20);

  Position add = p1 + p2;
  EXPECT_EQ(add.x, 15);
  EXPECT_EQ(add.y, 24);

  Position sub = p1 - p2;
  EXPECT_EQ(sub.x, 5);
  EXPECT_EQ(sub.y, 16);

  Position mul = p1 * p2;
  EXPECT_EQ(mul.x, 50);
  EXPECT_EQ(mul.y, 80);

  Position div = p1 / p2;
  EXPECT_EQ(div.x, 2);
  EXPECT_EQ(div.y, 5);

  Position mod = p1 % p2;
  EXPECT_EQ(mod.x, 0);
  EXPECT_EQ(mod.y, 0);
}

TEST(PositionTest, Containment) {
  proto::Rectangle rect;
  rect.set_x(10);
  rect.set_y(10);
  rect.set_width(100);
  rect.set_height(50);

  Position inside{20, 20};
  Position outside{5, 5};
  Position outside_far{200, 200};

  EXPECT_TRUE(inside.isContainedIn(rect));
  EXPECT_FALSE(outside.isContainedIn(rect));
  EXPECT_FALSE(outside_far.isContainedIn(rect));
}

TEST(SizeTest, ConversionsAndOperators) {
  Size sz{100, 200};
  wxSize wx_sz = sz.toWx();
  EXPECT_EQ(wx_sz.GetWidth(), 100);
  EXPECT_EQ(wx_sz.GetHeight(), 200);

  Size sz2 = Size::fromWx(wxSize(300, 400));
  EXPECT_EQ(sz2.width, 300);
  EXPECT_EQ(sz2.height, 400);

  EXPECT_TRUE(sz == sz);
  EXPECT_FALSE(sz == sz2);
  EXPECT_TRUE(sz != sz2);

  Size neg = -sz;
  EXPECT_EQ(neg.width, -100);
  EXPECT_EQ(neg.height, -200);

  Size add = sz + Size{50, 60};
  EXPECT_EQ(add.width, 150);
  EXPECT_EQ(add.height, 260);

  Size sub = sz - Size{50, 60};
  EXPECT_EQ(sub.width, 50);
  EXPECT_EQ(sub.height, 140);

  Size mul = sz * Size{2, 3};
  EXPECT_EQ(mul.width, 200);
  EXPECT_EQ(mul.height, 600);

  Size mul_double = sz * 2.5;
  EXPECT_EQ(mul_double.width, 250);
  EXPECT_EQ(mul_double.height, 500);

  Size div = sz / Size{2, 4};
  EXPECT_EQ(div.width, 50);
  EXPECT_EQ(div.height, 50);

  Size mod = sz % Size{30, 70};
  EXPECT_EQ(mod.width, 10);
  EXPECT_EQ(mod.height, 60);
}

}  // namespace cszb_scoreboard::test
