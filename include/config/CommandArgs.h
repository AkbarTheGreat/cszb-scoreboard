/*
config/CommandArgs.h: Parses and holds arguments that are passed in via the
command-line.

Copyright 2019-2022 Tracy Beck

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

#include <wx/cmdline.h>  // for wxCMD_LINE_SWITCH, wxCMD_LINE_NONE

#include <array>  // for array

#include "util/FilesystemPath.h"  // for FilesystemPath

class wxCmdLineArgsArray;

namespace cszb_scoreboard {
struct SingletonClass;

// Used in cszb-scoreboard, but defiend here to keep places to edit to a minimum
// when new flags are added.
static const std::array<wxCmdLineEntryDesc, 3> ARG_LIST{
    {{wxCMD_LINE_SWITCH, "n", "noupdate", "no automatic update checks"},
     {wxCMD_LINE_SWITCH, "r", "resetconfig",
      "reset all configuration to defaults"},
     {wxCMD_LINE_NONE}}};

class CommandArgs {
 public:
  explicit CommandArgs(SingletonClass c);
  /* parse must be called before getInstance, or an exception is thrown. */
  void process_args(const wxCmdLineParser &parser, int argc,
                    const wxCmdLineArgsArray &argv);

  // flag getters
  [[nodiscard]] auto autoUpdate() const -> bool;
  [[nodiscard]] auto resetConfig() const -> bool;
  auto commandPath() -> FilesystemPath;

 private:
  bool auto_update, reset_config;
  FilesystemPath command_path;
};
}  // namespace cszb_scoreboard
