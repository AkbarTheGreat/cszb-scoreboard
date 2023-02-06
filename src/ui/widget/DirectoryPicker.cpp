/*
ui/widget/DirectoryPicker.cpp: A modal (blocking other UI) dialog which pops up
to select a directory from the file system.

Copyright 2022-2023 Tracy Beck

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

#include "ui/widget/DirectoryPicker.h"

#include <wx/defs.h>    // for wxID_CANCEL
#include <wx/string.h>  // for wxString

#include <string>  // for allocator, string

#include "util/FilesystemPath.h"  // for FilesystemPath

namespace cszb_scoreboard {

DirectoryPicker::~DirectoryPicker() { delete _wx; }

auto DirectoryPicker::selectDirectory() -> std::optional<FilesystemPath> {
  if (_wx->ShowModal() == wxID_CANCEL) {
    return std::nullopt;
  }
  return FilesystemPath(std::string(_wx->GetPath()));
}

}  // namespace cszb_scoreboard
