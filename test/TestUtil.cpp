/*
test/TestUtil.cpp: A collection of test utilities which are used in multiple
tests as static methods.

Copyright 2020 Tracy Beck

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

#include "test/TestUtil.h"

namespace cszb_scoreboard {
namespace test {

const int TEST_ARGC = 1;
const char *TEST_BIN_NAME = "scoreboard_testing.exe";
const char *TEST_ARGV[TEST_ARGC] = {TEST_BIN_NAME};

void TestUtil::startApp(wxApp *app) {
  wxApp::SetInstance(app);
  // Argument to wxEntryStart cannot be const, so copy to a non-const before
  // calling
  int argc = TEST_ARGC;
  wxEntryStart(argc, (char **)TEST_ARGV);
  app->OnInit();
}

}  // namespace test
}  // namespace cszb_scoreboard
