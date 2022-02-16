/*
ui/widget/DragAndDropHandler.cpp: An object which handles receiving drops from
drag and drop operations.  Currently only supports receiving URLs from a
Browser.

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

#include "ui/widget/DragAndDropHandler.h"

namespace cszb_scoreboard {

HeldTarget::HeldTarget(
    const std::function<void(int32_t, int32_t, const std::string &)> &on_drop) {
  this->on_drop = on_drop;
  SetDataObject(new wxURLDataObject);
}

auto HeldTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
    -> wxDragResult {
  return wxDragCopy;
}

auto HeldTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
    -> wxDragResult {
  if (!GetData()) {
    return wxDragNone;
  }

  on_drop(static_cast<int32_t>(x), static_cast<int32_t>(y),
          std::string(static_cast<wxURLDataObject *>(m_dataObject)->GetURL()));
  return def;
}

bool HeldTarget::OnDrop(wxCoord x, wxCoord y) { return true; }

DragAndDropHandler ::DragAndDropHandler(
    Widget *owner,
    const std::function<void(int32_t, int32_t, const std::string &)> &on_drop) {
  held = new HeldTarget(on_drop);
  owner->wx()->SetDropTarget(held);
}

}  // namespace cszb_scoreboard
