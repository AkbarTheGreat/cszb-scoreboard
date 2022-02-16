/*
ui/widget/DragAndDropHandler.h: An object which handles receiving drops from
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

#pragma once

#include <wx/dataobj.h>

#include "ui/widget/Widget.h"
#include "ui/widget/swx/DropTarget.h"

namespace cszb_scoreboard {

class HeldTarget : public swx::DropTarget {
 public:
  explicit HeldTarget(const std::function<void(int32_t, int32_t,
                                               const std::string &)> &on_drop);
  auto OnData(wxCoord x, wxCoord y, wxDragResult def) -> wxDragResult override;
  auto OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
      -> wxDragResult override;
  auto OnDrop(wxCoord x, wxCoord y) -> bool override;

 private:
  std::function<void(int32_t, int32_t, std::string)> on_drop;
};

class DragAndDropHandler {
 public:
  explicit DragAndDropHandler(
      Widget *owner,
      const std::function<void(int32_t, int32_t, const std::string &)>
          &on_drop);

 private:
  HeldTarget *held;
};
}  // namespace cszb_scoreboard
