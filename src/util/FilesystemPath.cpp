/*
util/FilesystemPath.cpp: In most cases, a simple wrapper around
std::filesystem::path. For cases where that support is unavailable, a simple
stand-in which implements the functionality we need for our application.

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

#include "util/FilesystemPath.h"

#ifdef __APPLE__
#include <cstdio>
#endif

namespace cszb_scoreboard {

#ifdef __APPLE__

FilesystemPath::FilesystemPath() { path_string = ""; }

FilesystemPath::FilesystemPath(const std::string& str) { path_string = str; }

const int FilesystemPath::compare(const FilesystemPath& p) const {
  return path_string.compare(p.path_string);
}

bool FilesystemPath::remove(const FilesystemPath& p) {
  if (p.path_string == "") return false;
  // cstdio remove returns 0 for success, so return the negation of it's result
  return !std::remove(p.path_string.c_str());
}

void FilesystemPath::rename(const FilesystemPath& a, const FilesystemPath& b) {
  std::rename(a.path_string.c_str(), b.path_string.c_str());
}

const FilesystemPath FilesystemPath::filename() {
  std::size_t separator = path_string.rfind('/');
  std::string filename = path_string;
  if (separator != -1) {
    filename = path_string.substr(separator + 1, path_string.size() - 1);
  }
  return FilesystemPath(filename);
}

const FilesystemPath FilesystemPath::pathname() {
  std::size_t separator = path_string.rfind('/');
  std::string pathname = path_string;
  if (separator != -1) {
    pathname = path_string.substr(0, separator);
  }
  return FilesystemPath(pathname);
}

void FilesystemPath::replace_filename(const std::string new_filename) {
  path_string = pathname().string() + '/' + new_filename;
}

#endif

}  // namespace cszb_scoreboard
