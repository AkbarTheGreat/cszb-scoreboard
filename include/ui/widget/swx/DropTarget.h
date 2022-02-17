/*
ui/widget/swx/DropTarget.h: A wrapper around wxDropTarget.

Copyright 2022 Tracy Beck

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

#include <wx/dnd.h>
#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class DropTarget : public wxDropTarget {
 public:
  explicit DropTarget(wxDataObject *data = nullptr) : wxDropTarget(data) {}
};

}  // namespace cszb_scoreboard::swx
