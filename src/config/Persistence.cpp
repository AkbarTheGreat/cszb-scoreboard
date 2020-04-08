/*
config/Persistence.cpp: This class manages serializing/deserializing
our configuration proto to/from disk via a singleton.

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

#include "config/Persistence.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <wx/wx.h>

#include <fstream>
#include <iostream>

#include "config/CommandArgs.h"

namespace cszb_scoreboard {

// TODO: This is currently writing and reading binary files, I'd rather
// read/write textproto as that's easier for a person to inspect if weird things
// happen.
// #define TEXT_CONFIGURATION_FILES

const char* CONFIG_FILE = "scoreboard.config";
const char* IMAGE_LIBRARY_FILE = "image_library.data";

Persistence* Persistence::singleton_instance = nullptr;

Persistence* Persistence::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new Persistence();
    singleton_instance->loadConfigFromDisk();
    singleton_instance->loadImageLibraryFromDisk();
  }
  return singleton_instance;
}

#ifdef TEXT_CONFIGURATION_FILES
int openFileForWrite(const char* filename) {
  int descriptor;
  errno_t error = _sopen_s(&descriptor, filename, _O_CREAT | _O_TRUNC,
                           _SH_DENYWR, _S_IREAD | _S_IWRITE);
  return descriptor;
}

void closeFile(int descriptor) { _close(descriptor); }
#endif

void Persistence::loadConfigFromDisk() {
  if (CommandArgs::getInstance()->resetConfig()) {
    wxLogDebug(
        "Reset config argument passed, so all configuration data reset.");
    return;
  }
#ifdef TEXT_CONFIGURATION_FILES
  // TODO: Write code to load text files
#else
  std::fstream input(CONFIG_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    wxLogDebug("%s: File not found. Creating a default config.", CONFIG_FILE);
  } else if (!full_config.ParseFromIstream(&input)) {
    wxLogDebug("Failure parsing configuration file %s.", CONFIG_FILE);
  }
#endif
}

void Persistence::saveConfigToDisk() {
#ifdef TEXT_CONFIGURATION_FILES
  int descriptor = openFileForWrite(CONFIG_FILE);
  google::protobuf::io::FileOutputStream* fout =
      new google::protobuf::io::FileOutputStream(descriptor);
  bool print_status = google::protobuf::TextFormat::Print(full_config, fout);
  delete fout;
  closeFile(descriptor);
#else
  std::fstream output(CONFIG_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!full_config.SerializeToOstream(&output)) {
    wxLogDebug("Failed to write configuration file %s.", CONFIG_FILE);
  }
#endif
}

void Persistence::loadImageLibraryFromDisk() {
  std::fstream input(IMAGE_LIBRARY_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    wxLogDebug("%s: File not found. Creating an empty library.",
               IMAGE_LIBRARY_FILE);
  } else if (!image_library.ParseFromIstream(&input)) {
    wxLogDebug("Failure parsing image library file %s.", IMAGE_LIBRARY_FILE);
  }
}

void Persistence::saveImageLibraryToDisk() {
  std::fstream output(IMAGE_LIBRARY_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!image_library.SerializeToOstream(&output)) {
    wxLogDebug("Failed to write image library file %s.", IMAGE_LIBRARY_FILE);
  }
}

proto::DisplayConfig Persistence::loadDisplays() {
  // We don't actually have a way to reload after initialization at this point,
  // but that should be fine, as this should still represent what's written out.
  return full_config.display_config();
}

void Persistence::saveDisplays(const proto::DisplayConfig& display_config) {
  // full_config.clear_display_config();
  proto::DisplayConfig* new_display_config =
      full_config.mutable_display_config();
  new_display_config->CopyFrom(display_config);
  saveConfigToDisk();
}

proto::TeamConfig Persistence::loadTeams() {
  // We don't actually have a way to reload after initialization at this point,
  // but that should be fine, as this should still represent what's written out.
  return full_config.team_config();
}

void Persistence::saveTeams(const proto::TeamConfig& team_config) {
  // full_config.clear_display_config();
  proto::TeamConfig* new_team_config = full_config.mutable_team_config();
  new_team_config->CopyFrom(team_config);
  saveConfigToDisk();
}

proto::ImageLibrary Persistence::loadImageLibrary() {
  // There's currently no route to reload from disk and any attempt to save to
  // disk updates this variable, so for now this is sufficient.
  return image_library;
}

void Persistence::saveImageLibrary(const proto::ImageLibrary& library) {
  image_library.CopyFrom(library);
  saveImageLibraryToDisk();
}

}  // namespace cszb_scoreboard
