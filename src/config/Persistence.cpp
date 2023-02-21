/*
config/Persistence.cpp: This class manages serializing/deserializing
our configuration proto to/from disk via a singleton.

Copyright 2019-2023 Tracy Beck

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

#include <fstream>  // IWYU pragma: keep

#include "config/CommandArgs.h"  // IWYU pragma: keep
#include "util/Log.h"            // IWYU pragma: keep

namespace cszb_scoreboard {

#ifdef SCOREBOARD_TESTING
// For testing, do not read/write to disk.
#define FAKE_CONFIGURATION_FILES
#endif

const char *CONFIG_FILE = "scoreboard.config";
const char *IMAGE_LIBRARY_FILE = "image_library.data";
const char *TEAM_LIBRARY_FILE = "team_library.data";

Persistence::Persistence(SingletonClass c, Singleton *singleton) {
  this->singleton = singleton;
  loadConfigFromDisk();
  loadImageLibraryFromDisk();
  loadTeamLibraryFromDisk();
}

void Persistence::loadConfigFromDisk() {
#ifdef FAKE_CONFIGURATION_FILES
  // Reset config to default proto.
  full_config = proto::ScoreboardConfig();
#else
  if (singleton->commandArgs()->resetConfig()) {
    LogDebug("Reset config argument passed, so all configuration data reset.");
    return;
  }
  std::fstream input(CONFIG_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    LogDebug("%s: File not found. Creating a default config.", CONFIG_FILE);
  } else if (!full_config.ParseFromIstream(&input)) {
    LogDebug("Failure parsing configuration file %s.", CONFIG_FILE);
  }
#endif
}

void Persistence::saveConfigToDisk() {
#ifndef FAKE_CONFIGURATION_FILES
  //  Do not save if we're doing faked data
  std::fstream output(CONFIG_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!full_config.SerializeToOstream(&output)) {
    LogDebug("Failed to write configuration file %s.", CONFIG_FILE);
  }
#endif
}

void Persistence::loadImageLibraryFromDisk() {
#ifdef FAKE_CONFIGURATION_FILES
  // Create an empty image library.  In the future, we may want to populate test
  // data here.
  image_library = proto::ImageLibrary();
#else
  std::fstream input(IMAGE_LIBRARY_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    LogDebug("%s: File not found. Creating an empty library.",
             IMAGE_LIBRARY_FILE);
  } else if (!image_library.ParseFromIstream(&input)) {
    LogDebug("Failure parsing image library file %s.", IMAGE_LIBRARY_FILE);
  }
#endif
}

void Persistence::saveImageLibraryToDisk() {
#ifndef FAKE_CONFIGURATION_FILES
  //  Do not save if we're doing faked data
  std::fstream output(IMAGE_LIBRARY_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!image_library.SerializeToOstream(&output)) {
    LogDebug("Failed to write image library file %s.", IMAGE_LIBRARY_FILE);
  }
#endif
}

void Persistence::loadTeamLibraryFromDisk() {
#ifdef FAKE_CONFIGURATION_FILES
  // For testing, populate some default values.  These are just some teams we
  // use in Boston, they're placeholders.
  team_library = proto::TeamLibrary();
  proto::TeamLibInfo *home_team_info = team_library.add_teams();
  home_team_info->set_default_team_type(proto::TeamInfo_TeamType_HOME_TEAM);
  home_team_info->set_name("Rozzie Square Pegs");
  home_team_info->set_image_path("c:\\logos\\RSP.png");
  home_team_info->set_is_relative(false);
  proto::TeamLibInfo *away_team_info = team_library.add_teams();
  away_team_info->set_default_team_type(proto::TeamInfo_TeamType_AWAY_TEAM);
  away_team_info->set_name("Boston Baked Beans");
  away_team_info->set_image_path("BBB.png");
  away_team_info->set_is_relative(true);
  proto::TeamLibInfo *third_team_info = team_library.add_teams();
  third_team_info->set_name("Waltham Sandwiches");
  proto::TeamLibInfo *fourth_team_info = team_library.add_teams();
  fourth_team_info->set_name("Brookline NSynchers");
  // End testing stuff to be deleted
#else
  std::fstream input(TEAM_LIBRARY_FILE, std::ios::in | std::ios::binary);
  if (!input) {
    LogDebug("%s: File not found. Creating an empty library.",
             TEAM_LIBRARY_FILE);
  } else if (!team_library.ParseFromIstream(&input)) {
    LogDebug("Failure parsing team library file %s.", TEAM_LIBRARY_FILE);
  }
#endif
}

void Persistence::saveTeamLibraryToDisk() {
#ifndef FAKE_CONFIGURATION_FILES
  //  Do not save if we're doing faked data
  std::fstream output(TEAM_LIBRARY_FILE,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  if (!team_library.SerializeToOstream(&output)) {
    LogDebug("Failed to write team library file %s.", TEAM_LIBRARY_FILE);
  }
#endif
}

auto Persistence::loadDisplays() -> proto::DisplayConfig {
  // We don't actually have a way to reload from disk after initialization at
  // this point, but that should be fine, as this should still represent what's
  // written out.
  return full_config.display_config();
}

void Persistence::saveDisplays(const proto::DisplayConfig &display_config) {
  // full_config.clear_display_config();
  proto::DisplayConfig *new_display_config =
      full_config.mutable_display_config();
  new_display_config->CopyFrom(display_config);
  saveConfigToDisk();
}

auto Persistence::loadTeams() -> proto::TeamConfig {
  // We don't actually have a way to reload after initialization at this point,
  // but that should be fine, as this should still represent what's written out.
  return full_config.team_config();
}

void Persistence::saveTeams(const proto::TeamConfig &team_config) {
  // full_config.clear_display_config();
  proto::TeamConfig *new_team_config = full_config.mutable_team_config();
  new_team_config->CopyFrom(team_config);
  saveConfigToDisk();
}

auto Persistence::loadImageLibrary() -> proto::ImageLibrary {
  // There's currently no route to reload from disk and any attempt to save to
  // disk updates this variable, so for now this is sufficient.
  return image_library;
}

void Persistence::saveImageLibrary(const proto::ImageLibrary &library) {
  image_library.CopyFrom(library);
  saveImageLibraryToDisk();
}

auto Persistence::loadTeamLibrary() -> proto::TeamLibrary {
  // There's currently no route to reload from disk and any attempt to save to
  // disk updates this variable, so for now this is sufficient.
  return team_library;
}

void Persistence::saveTeamLibrary(const proto::TeamLibrary &library) {
  team_library.CopyFrom(library);
  saveTeamLibraryToDisk();
}

}  // namespace cszb_scoreboard
