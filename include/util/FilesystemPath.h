/*
util/Path.h: In most cases, a simple wrapper around std::filesystem::path. For
cases where that support is unavailable, a simple stand-in which implements the
functionality we need for our application.

Copyright 2020-2021 Tracy Beck

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

#include <string>
#ifdef __APPLE__
#include <string>
#else
#include <filesystem>
#endif

namespace cszb_scoreboard {

#ifdef __APPLE__
class FilesystemPath {
 public:
  FilesystemPath();
  explicit FilesystemPath(const std::string& str);

  static auto remove(const FilesystemPath& p) -> bool;
  static void rename(const FilesystemPath& a, const FilesystemPath& b);

  auto filename() -> FilesystemPath;
  auto pathname() -> FilesystemPath;
  void replace_filename(const std::string& new_filename);
  [[nodiscard]] auto string() const -> std::string { return path_string; }
  [[nodiscard]] auto c_str() const -> const char* {
    return path_string.c_str();
  }

  [[nodiscard]] auto compare(const FilesystemPath& p) const -> int;

  friend auto operator<(const FilesystemPath& a, const FilesystemPath& b)
      -> bool {
    return a.compare(b) < 0;
  }
  friend auto operator==(const FilesystemPath& a, const FilesystemPath& b)
      -> bool {
    return a.compare(b) == 0;
  }
  friend auto operator>(const FilesystemPath& a, const FilesystemPath& b)
      -> bool {
    return b < a;
  }
  friend auto operator<=(const FilesystemPath& a, const FilesystemPath& b)
      -> bool {
    return !(a > b);
  }
  friend auto operator>=(const FilesystemPath& a, const FilesystemPath& b)
      -> bool {
    return !(a < b);
  }
  friend auto operator!=(const FilesystemPath& a, const FilesystemPath& b)
      -> bool {
    return !(a == b);
  }

 private:
  std::string path_string;
};
#else
// Simply alias to std::filesystem::path for non-Mac platforms.
class FilesystemPath : public std::filesystem::path {
 public:
  FilesystemPath() = default;
  explicit FilesystemPath(const std::string& str)
      : std::filesystem::path(str) {}

  static auto remove(const FilesystemPath& p) -> bool {
    return std::filesystem::remove(p);
  }

  static void rename(const FilesystemPath& a, const FilesystemPath& b) {
    std::filesystem::rename(a, b);
  }
};
#endif

}  // namespace cszb_scoreboard
