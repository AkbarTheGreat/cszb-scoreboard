/*
test/GeneralConfigTest.cpp: Tests for config/GeneralConfig

Copyright 2024-2025 Tracy Beck

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

#include <memory>  // for unique_ptr, allocator

#include "config.pb.h"                          // for GeneralConfig
#include "config/GeneralConfig.h"               // for GeneralConfig
#include "gmock/gmock.h"                        // for Return, ReturnAction
#include "gtest/gtest.h"                        // for AssertionResult, Message
#include "test/mocks/config/MockPersistence.h"  // for MockPersistence
#include "test/mocks/util/MockSingleton.h"      // for MockSingleton
#include "util/Singleton.h"                     // for SingletonClass

namespace cszb_scoreboard::test {

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SaveArg;

class GeneralConfigTest : public ::testing::Test {
 protected:
  std::unique_ptr<proto::GeneralConfig> general_config;
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockPersistence> persist;

  GeneralConfigTest() = default;
  ~GeneralConfigTest() override = default;

  void SetUp() override {
    general_config = defaultConfig();
    singleton = std::make_unique<MockSingleton>();
    persist = std::make_unique<MockPersistence>(singleton.get());
    EXPECT_CALL(*singleton, persistence).WillRepeatedly(Return(persist.get()));
  }

  void TearDown() override {
    general_config.reset();
    singleton.reset();
    persist->resetGeneralConfig();
    persist.reset();
  }

  static auto defaultConfig() -> std::unique_ptr<proto::GeneralConfig> {
    auto config = std::make_unique<proto::GeneralConfig>();
    return config;
  }
};

TEST_F(GeneralConfigTest, Defaults) {
  GeneralConfig config(SingletonClass{}, singleton.get());
  EXPECT_EQ(-1, config.dropShadowDistance());
}

TEST_F(GeneralConfigTest, LoadSaveSettings) {
  const double shadowDistance = 5.0;
  proto::GeneralConfig initial_config;
  initial_config.set_drop_shadow_depth(shadowDistance);

  persist->saveGeneralConfig(initial_config);

  GeneralConfig generalConfig(SingletonClass{}, singleton.get());
  EXPECT_EQ(shadowDistance, generalConfig.dropShadowDistance());
}

TEST_F(GeneralConfigTest, SetGetDropShadowDistance) {
  GeneralConfig generalConfig(SingletonClass{}, singleton.get());
  EXPECT_EQ(-1, generalConfig.dropShadowDistance());

  double distance = 3.0;  // NOLINT (readability-magic-numbers)
  generalConfig.setDropShadowDistance(distance);
  EXPECT_EQ(distance, generalConfig.dropShadowDistance());
}

TEST_F(GeneralConfigTest, SaveSettings) {
  const double shadowDistance = 1.0;
  GeneralConfig initialConfig(SingletonClass{}, singleton.get());

  // Check that settings are not saved initially.
  EXPECT_FALSE(persist->loadGeneralConfig().drop_shadow_depth() ==
               shadowDistance);

  {
    auto generalConfig =
        std::make_unique<GeneralConfig>(SingletonClass{}, singleton.get());
    generalConfig->setDropShadowDistance(shadowDistance);
    generalConfig->saveSettings();
  }

  // Check that settings are saved after setting and saving them.
  EXPECT_TRUE(persist->loadGeneralConfig().drop_shadow_depth() ==
              shadowDistance);
}

}  // namespace cszb_scoreboard::test
