/*
cszb-scoreboard.cpp : Defines the entry point for the application.

Copyright 2019-2021 Tracy Beck

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

#include "cszb-scoreboard.h"

#include <wx/app.h>                 // for wxIMPLEMENT_APP
#include <wx/chartype.h>            // for wxT
#include <wx/cmdline.h>             // for wxCmdLineParser
#include <wx/gdicmn.h>              // for wxPoint, wxSize
#include <wx/image.h>               // for wxInitAllImageHandlers
#include <array>                    // for array

#include "config/CommandArgs.h"     // for ARG_LIST, CommandArgs
#include "ui/frame/FrameManager.h"  // for FrameManager
#include "ui/frame/MainView.h"      // for MainView
#include "util/Log.h"               // for LogDebug

#ifndef SCOREBOARD_TESTING
// NOLINTNEXTLINE(modernize-use-trailing-return-type)
wxIMPLEMENT_APP(cszb_scoreboard::Scoreboard);
#endif

namespace cszb_scoreboard {

static const int START_X = 50;
static const int START_Y = 50;
static const int START_WIDTH = 700;
static const int START_HEIGHT = 500;

auto Scoreboard::OnInit() -> bool {
  if (!wxApp::OnInit()) {
    return false;
  }
  wxInitAllImageHandlers();
  LogDebug(wxT("Starting up main loop"));
  FrameManager::getInstance()
      ->createMainView("ComedySportz Scoreboard", wxPoint(START_X, START_Y),
                       wxSize(START_WIDTH, START_HEIGHT))
      ->show(true);
  return true;
}

void Scoreboard::OnInitCmdLine(wxCmdLineParser& parser) {
  wxApp::OnInitCmdLine(parser);
  parser.SetDesc(ARG_LIST.data());
  // If we ever want '/' as a path separator to work in Windows, we have to
  // ignore / as a flag start, unfortunately.  Not an issue yet, but may be in
  // the future.   If we need it, set via parser.SetSwitchChars(wxT("-"));
}

auto Scoreboard::OnCmdLineParsed(wxCmdLineParser& parser) -> bool {
  if (!CommandArgs::process_args(parser, argc, argv)) {
    return false;
  }
  return wxApp::OnCmdLineParsed(parser);
}

}  // namespace cszb_scoreboard
