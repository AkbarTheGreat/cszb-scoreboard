/*
ui/component/MockPreviewPanel.h: Mock PreviewPanel implementation for testing.

Copyright 2021-2026 Tracy Beck

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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ui/component/PreviewPanel.h"

// IWYU pragma: no_include "gmock/gmock.h"
// IWYU pragma: no_include "gtest/gtest.h"
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard::test {

class MockPreviewPanel : public PreviewPanel {
 public:
  MockFrameManager() : FrameManager(SingletonClass{}) {}

  MOCK_METHOD(MainView*, createMainView,
              (const std::string& title, const Position& pos, const Size& size),
              (override));
  MOCK_METHOD(ScreenPresenter*, createScreenPresenter,
              (int monitor_number, const ScreenText& preview), (override));
  MOCK_METHOD(MainView*, mainView, (), (override));
  MOCK_METHOD(void, exitFrames, (), (override));
  MOCK_METHOD(int32_t, monitorCount, (), (override));
  MOCK_METHOD(Display, monitor, (uint32_t index), (override));
};

}  // namespace cszb_scoreboard::test
