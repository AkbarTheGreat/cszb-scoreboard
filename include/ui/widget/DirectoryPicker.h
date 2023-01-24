/*
ui/widget/DirectoryPicker.h: A modal (blocking other UI) dialog which pops up to
select a directory from the file system.

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

#include <optional>  // for optional

#include "ui/widget/Widget.h"  // for Widget
#include "ui/widget/swx/DirDialog.h"

class wxWindow;

namespace cszb_scoreboard {
class FilesystemPath;

class DirectoryPicker : public Widget {
 public:
  explicit DirectoryPicker(swx::DirDialog *dialog) { _wx = dialog; }
  ~DirectoryPicker();

  auto selectDirectory() -> std::optional<FilesystemPath>;

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }
  swx::DirDialog *_wx;
};

}  // namespace cszb_scoreboard