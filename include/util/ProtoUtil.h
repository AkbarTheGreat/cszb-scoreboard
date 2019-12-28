/*
util/ProtoUtil.h: Convenience methods for converting between proto values and
wxWidgets values.

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

#include <wx/wx.h>

#include "proto/config.pb.h"

namespace cszb_scoreboard {

class ProtoUtil {
 public:
  static proto::Rectangle *protoRct(wxRect input, proto::Rectangle *output);
  static wxRect wxRct(proto::Rectangle input);
};

}  // namespace cszb_scoreboard
