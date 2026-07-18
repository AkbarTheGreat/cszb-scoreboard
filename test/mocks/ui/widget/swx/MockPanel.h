/*
ui/widget/swx/MockPanel.h: Mock Panel implementation for testing.

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

#include "ui/widget/swx/Panel.h"

// IWYU pragma: no_include "gmock/gmock.h"
// IWYU pragma: no_include "gtest/gtest.h"
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard::test::swx {

class MockPanel : public cszb_scoreboard::swx::Panel {
 public:
  MockPanel() : Panel() {}

  MOCK_METHOD(bool, Close, (bool force), (override));
  MOCK_METHOD(wxStatusBar*, CreateStatusBar,
              (int number, int64_t style, wxWindowID id, const wxString& name),
              (override));
  MOCK_METHOD(bool, Destroy, (), (override));
  MOCK_METHOD(wxPoint, GetPosition, (), (const, override));
  MOCK_METHOD(int64_t, GetWindowStyle, (), (const, override));
  MOCK_METHOD(void, Iconize, (bool iconize), (override));
  MOCK_METHOD(void, SetAcceleratorTable, (const wxAcceleratorTable& accel),
              (override));
  MOCK_METHOD(void, SetMenuBar, (wxMenuBar * menuBar), (override));
  MOCK_METHOD(void, SetPosition, (const wxPoint& pt), (override));
  MOCK_METHOD(void, SetSize, (const wxSize& size), (override));
  MOCK_METHOD(void, SetStatusText, (const wxString& text, int number),
              (override));
  MOCK_METHOD(void, SetWindowStyle, (int64_t style), (override));
  MOCK_METHOD(bool, Show, (bool show), (override));
  MOCK_METHOD(bool, ShowFullScreen, (bool show, int64_t style), (override));
  MOCK_METHOD(void, Update, (), (override));
};

}  // namespace cszb_scoreboard::test::swx
