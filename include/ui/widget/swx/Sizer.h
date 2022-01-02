/*
ui/widget/swx/Sizer.h: A wrapper around wxGridBagSizer, for use as a default
sizer.

Copyright 2021-2022 Tracy Beck

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

#include <wx/gbsizer.h>
#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class Sizer : public wxGridBagSizer {
 public:
  explicit Sizer(int vgap = 0, int hgap = 0) : wxGridBagSizer(vgap, hgap) {}
};

}  // namespace cszb_scoreboard::swx
