/*
ui/widget/FilePicker.h: A modal (blocking other UI) dialog which pops up to
select a file from the file system.

Copyright 2021 Tracy Beck

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

#include <optional>

#include "ui/widget/Widget.h"
#include "ui/widget/swx/FileDialog.h"
#include "util/FilesystemPath.h"

namespace cszb_scoreboard {

class FilePicker : public Widget {
 public:
  explicit FilePicker(swx::FileDialog *dialog) { wx = dialog; }
  ~FilePicker();

  auto selectFile() -> std::optional<FilesystemPath>;

 protected:
  [[nodiscard]] auto _wx() const -> wxWindow * override { return wx; }
  swx::FileDialog *wx;
};

}  // namespace cszb_scoreboard
