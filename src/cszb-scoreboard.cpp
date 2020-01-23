/*
cszb-scoreboard.cpp : Defines the entry point for the application.

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

#include "cszb-scoreboard.h"

#include "config/CommandArgs.h"

#ifndef SCOREBOARD_TESTING
wxIMPLEMENT_APP(cszb_scoreboard::Scoreboard);
#endif

namespace cszb_scoreboard {

bool Scoreboard::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }

  wxInitAllImageHandlers();
  wxLogDebug(wxT("Starting up main loop"));
  main_window = new MainView("ComedySportz Scoreboard", wxPoint(50, 50),
                             wxSize(700, 500));
  main_window->Show(true);
  return true;
}

void Scoreboard::OnInitCmdLine(wxCmdLineParser& parser) {
  wxApp::OnInitCmdLine(parser);
  parser.SetDesc(ARG_LIST);
  // If we ever want '/' as a path separator to work in Windows, we have to
  // ignore / as a flag start, unfortunately.  Not an issue yet, but may be in
  // the future.   If we need it, set via parser.SetSwitchChars(wxT("-"));
}

bool Scoreboard::OnCmdLineParsed(wxCmdLineParser& parser) {
  if (!CommandArgs::process_args(parser, argc, argv)) {
    return false;
  }
  return wxApp::OnCmdLineParsed(parser);
}

}  // namespace cszb_scoreboard
