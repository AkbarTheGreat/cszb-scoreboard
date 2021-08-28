/*
test/TestUtil.cpp: A collection of test utilities which are used in multiple
tests as static methods.

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

#include "test/TestUtil.h"

#include <wx/app.h>   // IWYU pragma: keep for wxApp
#include <wx/init.h>  // for wxEntryStart

#include <array>  // for array
// IWYU pragma: no_include <wx/gtk/app.h>

namespace cszb_scoreboard ::test {

const std::array<const char *, 2> TEST_ARGV = {
    {{"scoreboard_testing.exe"}, {"-n"}}};

void TestUtil::startApp(wxApp *app) {
  wxApp::SetInstance(app);
  // Argument to wxEntryStart cannot be const, so copy to a non-const before
  // calling.
  int argc = TEST_ARGV.size();
  wxEntryStart(argc, const_cast<char **>(TEST_ARGV.data()));
  app->OnInit();
}

}  // namespace cszb_scoreboard::test
