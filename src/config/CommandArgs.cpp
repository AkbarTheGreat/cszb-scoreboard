/*
config/CommandArgs.cpp: Parses and holds arguments that are passed in via the
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

#include "config/CommandArgs.h"

#include <wx/chartype.h>  // for wxT
#include <wx/cmdargs.h>   // for wxCmdLineArgsArray
#include <wx/string.h>    // for wxCStrData, wxString

#include <memory>  // for allocator

#include "util/Singleton.h"  // for SingletonClass

namespace cszb_scoreboard {

void CommandArgs::process_args(const wxCmdLineParser &parser, int argc,
                               const wxCmdLineArgsArray &argv) {
  // TODO(akbar): This is always an absolute path on Windows, but may be a
  // relative path on Linux.  An update to always get an absolute path is
  // probably worthwhile.
  command_path = FilesystemPath(static_cast<const char *>(argv[0].c_str()));
  auto_update = !parser.Found(wxT("n"));
  reset_config = parser.Found(wxT("r"));
}

CommandArgs::CommandArgs(SingletonClass c) {
  // These defaults should never matter, as they're always set during
  // process_args.
  auto_update = true;
  reset_config = false;
}

auto CommandArgs::autoUpdate() const -> bool { return auto_update; }
// For consistency in call sites, leave this as a non-static method.
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto CommandArgs::enableImageSearch() const -> bool {
#ifdef _WIN32
  return true;
#endif  // #ifdef _WIN32
  return false;
}
auto CommandArgs::resetConfig() const -> bool { return reset_config; }
auto CommandArgs::commandPath() -> FilesystemPath { return command_path; }

}  // namespace cszb_scoreboard
