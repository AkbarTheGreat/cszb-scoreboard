/*
ui/widget/swx/MockFrame.h: Mock Frame implementation for testing.

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

#pragma once

#include "gmock/gmock.h"
#include "ui/widget/swx/Frame.h"

namespace cszb_scoreboard::test::swx {

class MockFrame : public cszb_scoreboard::swx::Frame {
 public:
  MockFrame() : Frame() {}

  MOCK_METHOD(bool, Close, (bool force), (override));
  MOCK_METHOD(wxStatusBar *, CreateStatusBar,
              (int number, long style, wxWindowID id, const wxString &name),
              (override));
  MOCK_METHOD(bool, Destroy, (), (override));
  MOCK_METHOD(wxPoint, GetPosition, (), (const, override));
  MOCK_METHOD(long, GetWindowStyle, (), (const, override));
  MOCK_METHOD(void, Iconize, (bool iconize), (override));
  MOCK_METHOD(void, SetAcceleratorTable, (const wxAcceleratorTable &accel),
              (override));
  MOCK_METHOD(void, SetMenuBar, (wxMenuBar * menuBar), (override));
  MOCK_METHOD(void, SetPosition, (const wxPoint &pt), (override));
  MOCK_METHOD(void, SetSize, (const wxSize &size), (override));
  MOCK_METHOD(void, SetStatusText, (const wxString &text, int number),
              (override));
  MOCK_METHOD(void, SetWindowStyle, (long style), (override));
  MOCK_METHOD(bool, Show, (bool show), (override));
  MOCK_METHOD(bool, ShowFullScreen, (bool show, long style), (override));
  MOCK_METHOD(void, Update, (), (override));
};

}  // namespace cszb_scoreboard::test::swx
