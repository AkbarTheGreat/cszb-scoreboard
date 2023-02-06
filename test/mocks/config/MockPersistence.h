/*
util/MockSingleton.h: Mock Singleton provider for testing.

Copyright 2021-2023 Tracy Beck

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

#pragma once

#include "config/Persistence.h"
#include "gmock/gmock.h"
#include "test/mocks/util/MockSingleton.h"

namespace cszb_scoreboard::test {

class MockPersistence : public Persistence {
 public:
  MockPersistence(MockSingleton *singleton)
      : Persistence(SingletonClass{}, singleton) {}

  MOCK_METHOD(proto::DisplayConfig, loadDisplays, (), (override));
  MOCK_METHOD(void, saveDisplays, (const proto::DisplayConfig &display_config),
              (override));
  MOCK_METHOD(proto::TeamConfig, loadTeams, (), (override));
  MOCK_METHOD(void, saveTeams, (const proto::TeamConfig &team_config),
              (override));
  MOCK_METHOD(proto::ImageLibrary, loadImageLibrary, (), (override));
  MOCK_METHOD(void, saveImageLibrary, (const proto::ImageLibrary &library),
              (override));
};

}  // namespace cszb_scoreboard::test
