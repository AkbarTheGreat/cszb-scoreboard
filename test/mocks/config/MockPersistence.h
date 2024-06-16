/*
util/MockSingleton.h: Mock Singleton provider for testing.

Copyright 2021-2024 Tracy Beck

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

/*
The MOCK_PERSISTENCE_METHODS macro sets up a local variable and three methods
that will fake out persistence locally to the object. The local variable is
assumed to be a protobuf of type {proto}.  The two methods of load{method_base}
and save{method_base} are overrides of the Persistence class.  A third method,
reset{method_base}, is added to reset the faked persistence.
*/

#define MOCK_PERSISTENCE_METHODS(method_base, proto)                       \
 private:                                                                  \
  proto local_##method_base;                                               \
                                                                           \
 public:                                                                   \
  auto load##method_base()->proto override { return local_##method_base; } \
  void save##method_base(const proto &var) override {                      \
    local_##method_base = var;                                             \
  }                                                                        \
  void reset##method_base() { local_##method_base = {}; }

class MockPersistence : public Persistence {
 public:
  MockPersistence(MockSingleton *singleton)
      : Persistence(SingletonClass{}, singleton) {}

  MOCK_PERSISTENCE_METHODS(Displays, proto::DisplayConfig);
  MOCK_PERSISTENCE_METHODS(GeneralConfig, proto::GeneralConfig);
  MOCK_PERSISTENCE_METHODS(ImageLibrary, proto::ImageLibrary);
  MOCK_PERSISTENCE_METHODS(Teams, proto::TeamConfig);
  MOCK_PERSISTENCE_METHODS(TeamLibrary, proto::TeamLibrary);
};

}  // namespace cszb_scoreboard::test
