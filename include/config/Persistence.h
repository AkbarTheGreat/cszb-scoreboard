/*
config/Persistence.h: This class manages serializing/deserializing
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
#pragma once

#include <google/protobuf/util/time_util.h>
#include <wx/wx.h>
#include <fstream>
#include <iostream>
#include "proto/config.pb.h"

namespace cszb_scoreboard {

class Persistence {
 public:
  proto::DisplayConfig loadDisplays();
  void saveDisplays(proto::DisplayConfig display_config);
};

}  // namespace cszb_scoreboard
