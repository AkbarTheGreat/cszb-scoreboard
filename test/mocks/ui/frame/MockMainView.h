/*
ui/frame/MockMainView.h: Mock MainView implementation for testing.

Copyright 2021-2022 Tracy Beck

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
#include "test/mocks/ui/widget/swx/MockFrame.h"
#include "test/mocks/util/MockSingleton.h"
#include "ui/frame/MainView.h"

// Empty initialization for testing.
void cszb_scoreboard::MainView::init(){};

namespace cszb_scoreboard::test {

class MockMainView : public MainView {
 public:
  MockMainView(swx::MockFrame* frame, MockSingleton* singleton)
      : MainView(frame, singleton) {}

  MOCK_METHOD(void, refreshPreviews, (), (override));
};

}  // namespace cszb_scoreboard::test
