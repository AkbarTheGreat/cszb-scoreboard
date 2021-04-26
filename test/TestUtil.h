/*
test/TestUtil.h: A collection of test utilities which are used in multiple tests
as static methods.

Copyright 2020-2021 Tracy Beck

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

#include <wx/wx.h>

namespace cszb_scoreboard ::test {

class TestUtil {
 public:
  static void startApp(wxApp *app);
};

}  // namespace cszb_scoreboard::test
