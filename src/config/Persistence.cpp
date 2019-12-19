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

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <io.h>
#include <wx/wx.h>

#include <fstream>
#include <iostream>

// TODO: This is currently writing and reading binary files, I'd rather
// read/write textproto as that's easier for a person to inspect if weird things
// happen.
// #define TEXT_CONFIGURATION_FILES

const char* CONFIG_FILE = "scoreboard.config";

namespace cszb_scoreboard {

#ifdef TEXT_CONFIGURATION_FILES
int openFileForWrite(const char* filename) {
  int descriptor;
  errno_t error = _sopen_s(&descriptor, filename, _O_CREAT | _O_TRUNC,
                           _SH_DENYWR, _S_IREAD | _S_IWRITE);
  return descriptor;
}

void closeFile(int descriptor) { _close(descriptor); }
#endif

proto::DisplayConfig Persistence::loadDisplays() {
  proto::DisplayConfig display_config;
#ifdef TEXT_CONFIGURATION_FILES
  // TODO: Manage loading text files
#else
  std::fstream input(CONFIG_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    wxLogDebug("%s: File not found. Creating a default config.", CONFIG_FILE);
  } else if (!display_config.ParseFromIstream(&input)) {
    wxLogDebug("Failure parsing configuration file %s.", CONFIG_FILE);
  }
#endif
  return display_config;
}

void Persistence::saveDisplays(const proto::DisplayConfig& display_config) {
#ifdef TEXT_CONFIGURATION_FILES
  int descriptor = openFileForWrite(CONFIG_FILE);
  google::protobuf::io::FileOutputStream* fout =
      new google::protobuf::io::FileOutputStream(descriptor);
  bool print_status = google::protobuf::TextFormat::Print(display_config, fout);
  delete fout;
  closeFile(descriptor);
#else
  std::fstream output(CONFIG_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!display_config.SerializeToOstream(&output)) {
    wxLogDebug("Failed to write configuration file %s.", CONFIG_FILE);
  }
#endif
}

}  // namespace cszb_scoreboard
