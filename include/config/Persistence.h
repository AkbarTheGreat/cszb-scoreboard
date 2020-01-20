/*
config/Persistence.h: This class manages serializing/deserializing
our configuration proto to/from disk via a singleton.

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
#pragma once

#include "config.pb.h"

namespace cszb_scoreboard {

class Persistence {
 public:
  static Persistence* getInstance();
  proto::DisplayConfig loadDisplays();
  void saveDisplays(const proto::DisplayConfig& display_config);
  proto::TeamConfig loadTeams();
  void saveTeams(const proto::TeamConfig& team_config);

 private:
  static Persistence* singleton_instance;
  proto::ScoreboardConfig full_config;
  void loadFromDisk();
  void saveToDisk();
};

}  // namespace cszb_scoreboard
