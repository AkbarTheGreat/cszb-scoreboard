/*
config/CommandArgs.cpp: Parses and holds arguments that are passed in via the
command-line.

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

#include "config/CommandArgs.h"

#include <stdexcept>

namespace cszb_scoreboard {

CommandArgs *CommandArgs::singleton_instance = nullptr;

CommandArgs *CommandArgs::getInstance() {
  if (singleton_instance == nullptr) {
    throw new std::runtime_error(
        "Cannot call getInstance on an unparsed CommandArgs object");
  }
  return singleton_instance;
}

bool CommandArgs::process_args(const wxCmdLineParser &parser) {
  if (singleton_instance != nullptr) {
    throw new std::runtime_error(
        "Cannot call process_args on an initialized CommandArgs object");
  }
  singleton_instance = new CommandArgs();
  return singleton_instance->process_args_internal(parser);
}

bool CommandArgs::process_args_internal(const wxCmdLineParser &parser) {
  auto_update = !parser.Found(wxT("n"));
  reset_config = !parser.Found(wxT("r"));

  return true;
}

CommandArgs::CommandArgs() {
  // These defaults should never matter, as they're always set during
  // process_args.
  auto_update = true;
  reset_config = false;
}

bool CommandArgs::autoUpdate() { return auto_update; }
bool CommandArgs::resetConfig() { return reset_config; }

}  // namespace cszb_scoreboard
