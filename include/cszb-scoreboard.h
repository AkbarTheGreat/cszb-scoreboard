/*
cszb-scoreboard.h: Include for main program for the scoreboard.

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

#pragma once

#include <wx/app.h>  // IWYU pragma: keep for wxApp
// IWYU pragma: no_include <wx/gtk/app.h>
#include "include/ScoreboardCommon.h"

class wxCmdLineParser;

namespace cszb_scoreboard {
class Scoreboard : public wxApp {
 public:
  auto OnInit() -> bool final;

  PUBLIC_TEST_ONLY
  static void close();

 private:
  void OnInitCmdLine(wxCmdLineParser &parser) final;
  auto OnCmdLineParsed(wxCmdLineParser &parser) -> bool final;
};
}  // namespace cszb_scoreboard
