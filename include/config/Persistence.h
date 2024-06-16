/*
config/Persistence.h: This class manages serializing/deserializing
our configuration proto to/from disk via a singleton.

Copyright 2019-2024 Tracy Beck

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

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "image_library.pb.h"
#include "team_library.pb.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {

class Persistence {
 public:
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singelton object, as it
  // allows mocking of singletons.
  explicit Persistence(SingletonClass c)
      : Persistence(c, Singleton::getInstance()) {}
  // GCOVR_EXCL_STOP
  virtual auto loadDisplays() -> proto::DisplayConfig;
  virtual void saveDisplays(const proto::DisplayConfig &display_config);
  virtual auto loadGeneralConfig() -> proto::GeneralConfig;
  virtual void saveGeneralConfig(const proto::GeneralConfig &general_config);
  virtual auto loadTeams() -> proto::TeamConfig;
  virtual void saveTeams(const proto::TeamConfig &team_config);
  virtual auto loadImageLibrary() -> proto::ImageLibrary;
  virtual void saveImageLibrary(const proto::ImageLibrary &library);
  virtual auto loadTeamLibrary() -> proto::TeamLibrary;
  virtual void saveTeamLibrary(const proto::TeamLibrary &library);

  PUBLIC_TEST_ONLY
  Persistence(SingletonClass c, Singleton *singleton);

 private:
  proto::ScoreboardConfig full_config;
  proto::ImageLibrary image_library;
  proto::TeamLibrary team_library;
  Singleton *singleton;
  void loadConfigFromDisk();
  void saveConfigToDisk();
  void loadImageLibraryFromDisk();
  void saveImageLibraryToDisk();
  void loadTeamLibraryFromDisk();
  void saveTeamLibraryToDisk();
};

}  // namespace cszb_scoreboard
