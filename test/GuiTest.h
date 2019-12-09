/*
test/GuiTest.h: A test utility for unit testing wxWidget functionality.  The
idea for this methodology for testing wxWidgets via GoogleTest originally found
at http://www.remy.org.uk/tech.php?tech=1407951209, rewritten here with several
improvements and modernizations.

Copyright 2019 Tracy Beck

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

#include <gtest/gtest.h>
#include <wx/uiaction.h>
#include <wx/wx.h>

#include "cszb-scoreboard.h"
#include "ui/FrameList.h"
#include "ui/MainView.h"

namespace cszb_scoreboard {

/* Performs an action against the wxWidgets UI, and yields to allow it to
 * execute */
#define WX_A(action) \
  {                  \
    action;          \
    wxYield();       \
  }

const int TEST_ARGC = 1;
const char *TEST_BIN_NAME = "scoreboardUnitTest.exe";
const char *TEST_ARGV[TEST_ARGC] = {TEST_BIN_NAME};

class GuiTest : public testing::Test {
 protected:
  Scoreboard *app;
  wxUIActionSimulator act;

  virtual void SetUp() override {
    app = new Scoreboard();
    wxApp::SetInstance(app);
    // Argument to wxEntryStart cannot be const, so copy to a non-const before
    // calling
    int argc = TEST_ARGC;
    wxEntryStart(argc, (char **)TEST_ARGV);
    app->OnInit();
    mainView()->Update();
  }

  virtual void TearDown() override {
    app->OnExit();
    wxEntryCleanup();
  }

  /* Convenience method to get the main window, for testing purposes. */
  MainView *mainView() { return (MainView*)FrameList::getInstance()->getMainView(); }
};

}  // namespace cszb_scoreboard
