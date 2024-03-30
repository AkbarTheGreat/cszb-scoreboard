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

#include "gmock/gmock.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {
namespace test {

class MockSingleton : public Singleton {
 public:
  MOCK_METHOD(AutoUpdate*, autoUpdate, (), (override));
  MOCK_METHOD(CommandArgs*, commandArgs, (), (override));
  MOCK_METHOD(DisplayConfig*, displayConfig, (), (override));
  MOCK_METHOD(FrameManager*, frameManager, (), (override));
  MOCK_METHOD(GeneralConfig*, generalConfig, (), (override));
  MOCK_METHOD(HotkeyTable*, hotkeyTable, (), (override));
  MOCK_METHOD(ImageLibrary*, imageLibrary, (), (override));
  MOCK_METHOD(Persistence*, persistence, (), (override));
  MOCK_METHOD(TeamColors*, teamColors, (), (override));
  MOCK_METHOD(TeamConfig*, teamConfig, (), (override));
  MOCK_METHOD(TimerManager*, timerManager, (), (override));

  MOCK_METHOD(void, generateCommandArgs,
              (const wxCmdLineParser& parser, int argc,
               const wxCmdLineArgsArray& argv),
              (override));

  ~MockSingleton(){};
};

}  // namespace test

#ifndef USE_REAL_SINGLETON
auto Singleton::getInstance() -> Singleton* {
  static test::MockSingleton instance;
  return &instance;
}
#endif

}  // namespace cszb_scoreboard
