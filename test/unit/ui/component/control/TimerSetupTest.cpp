/*
test/unit/ui/component/control/TimerSetupTest.cpp: Tests for
ui/component/control/TimerSetup

Copyright 2021 Tracy Beck

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

#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>  // IWYU pragma: keep

#include "test/TestUtil.h"
#include "ui/component/control/TimerSetup.h"

// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

using ::testing::_;

namespace cszb_scoreboard ::test {

class TimerSetupTest : public ::testing::Test {
 protected:
  void SetUp() override {
    /*
    EXPECT_CALL(*main_view_frame, Destroy)
        .WillRepeatedly(
            Return(true));
            */
  }

  void TearDown() override {}
};

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST_F(TimerSetupTest, TimerLengthCalculation) {
  auto setup =
      TimerSetup::Create(/*PreviewPanel * */ nullptr, /* swx::Panel * */ nullptr);
  //EXPECT_EQ(3, autoConfig.numberOfDisplays());
}

}  // namespace cszb_scoreboard::test
