/*
config/Persistence.cpp: This class manages serializing/deserializing
our configuration proto to/from disk

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

#include "config/Persistence.h"

const char* CONFIG_FILE = "scoreboard.textproto";

namespace cszb_scoreboard {

proto::DisplayConfig Persistence::loadDisplays() {
  // Read the existing address book.
  proto::DisplayConfig display_config;
  std::fstream input(CONFIG_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    wxLogDebug("%s: File not found. Creating a default config.", CONFIG_FILE);
  } else if (!display_config.ParseFromIstream(&input)) {
    wxLogDebug("Failure parsing configuration file %s.", CONFIG_FILE);
  }
  return display_config;
}

void saveDisplays(proto::DisplayConfig display_config) {
  std::fstream output(CONFIG_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  // TODO: This is currently writing and reading binary files, I'd rather
  // read/write textproto (as the file is named) as that's easier for a person
  // to inspect if weird things happen.
  if (!display_config.SerializeToOstream(&output)) {
    wxLogDebug("Failed to write configuration file %s.", CONFIG_FILE);
  }
}

}  // namespace cszb_scoreboard
