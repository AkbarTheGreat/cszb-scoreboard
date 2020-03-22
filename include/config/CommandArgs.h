/*
config/CommandArgs.h: Parses and holds arguments that are passed in via the
command-line.

Copyright 2019-2020 Tracy Beck

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

#include <wx/cmdline.h>

#include "util/FilesystemPath.h";

namespace cszb_scoreboard {

// Used in cszb-scoreboard, but defiend here to keep places to edit to a minimum
// when new flags are added.
static const wxCmdLineEntryDesc ARG_LIST[] = {
    {wxCMD_LINE_SWITCH, "n", "noupdate", "no automatic update checks"},
    {wxCMD_LINE_SWITCH, "r", "resetconfig",
     "reset all configuration to defaults"},
    {wxCMD_LINE_NONE}};

class CommandArgs {
 public:
  static CommandArgs *getInstance();
  /* parse must be called before getInstance, or an exception is thrown. */
  static bool process_args(const wxCmdLineParser &parser, int argc,
                           const wxCmdLineArgsArray &argv);

  // flag getters
  bool autoUpdate();
  bool resetConfig();
  FilesystemPath commandPath();

 private:
  static CommandArgs *singleton_instance;
  bool auto_update, reset_config;
  FilesystemPath command_path;

  CommandArgs();
  bool process_args_internal(const wxCmdLineParser &parser, int argc,
                             const wxCmdLineArgsArray &argv);
};
}  // namespace cszb_scoreboard
