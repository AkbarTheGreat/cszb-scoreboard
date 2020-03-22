/*
util/Path.h: In most cases, a simple wrapper around std::filesystem::path. For
cases where that support is unavailable, a simple stand-in which implements the
functionality we need for our application.

Copyright 2020 Tracy Beck

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

#ifndef __APPLE__
#include <filesystem>
#endif

namespace cszb_scoreboard {

#ifdef __APPLE__
class FilesystemPath {
  // TODO: Put OSX implementation of our class here.
 public:
  static bool remove(const FilesystemPath& p) {}
  static void rename(const FilesystemPath& a, const FilesystemPath& b) {}
};
#else
// Simply alias to std::filesystem::path for non-Mac platforms.
class FilesystemPath : public std::filesystem::path {
 public:
  FilesystemPath() : std::filesystem::path() {}
  FilesystemPath(const std::string& str) : std::filesystem::path(str) {}

  static bool remove(const FilesystemPath& p) {
    return std::filesystem::remove(p);
  }

  static void rename(const FilesystemPath& a, const FilesystemPath& b) {
    std::filesystem::rename(a, b);
  }
};
#endif

}  // namespace cszb_scoreboard
