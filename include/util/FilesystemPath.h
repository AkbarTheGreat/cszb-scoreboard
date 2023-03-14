/*
util/FilesystemPath.h: In most cases, a moderately enhanced wrapper around
std::filesystem::path. For cases where that support is unavailable, a simple
stand-in which implements the functionality we need for our application.

Copyright 2020-2023 Tracy Beck

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

#ifdef __APPLE__
#define SCOREBOARD_APPLE_IMPL
#endif

#include <string>
#ifdef SCOREBOARD_APPLE_IMPL
#include <string>
#else
#include <filesystem>
#endif

namespace cszb_scoreboard {

#ifdef SCOREBOARD_APPLE_IMPL
class FilesystemPath {
 public:
  FilesystemPath();
  explicit FilesystemPath(const std::string &str);

  static auto remove(const FilesystemPath &p) -> bool;
  static void rename(const FilesystemPath &a, const FilesystemPath &b);

  auto filename() -> FilesystemPath;
  auto pathname() -> FilesystemPath;
  void replace_filename(const std::string &new_filename);
  [[nodiscard]] auto string() const -> std::string { return path_string; }
  [[nodiscard]] auto c_str() const -> const char * {
    return path_string.c_str();
  }

  [[nodiscard]] auto compare(const FilesystemPath &p) const -> int;

  [[nodiscard]] auto is_absolute() const -> bool {
    return path_string[0] == '/';
  }

  [[nodiscard]] auto is_relative() const -> bool { return !is_absolute(); }

  friend auto operator<(const FilesystemPath &a, const FilesystemPath &b)
      -> bool {
    return a.compare(b) < 0;
  }
  friend auto operator==(const FilesystemPath &a, const FilesystemPath &b)
      -> bool {
    return a.compare(b) == 0;
  }
  friend auto operator>(const FilesystemPath &a, const FilesystemPath &b)
      -> bool {
    return b < a;
  }
  friend auto operator<=(const FilesystemPath &a, const FilesystemPath &b)
      -> bool {
    return !(a > b);
  }
  friend auto operator>=(const FilesystemPath &a, const FilesystemPath &b)
      -> bool {
    return !(a < b);
  }
  friend auto operator!=(const FilesystemPath &a, const FilesystemPath &b)
      -> bool {
    return !(a == b);
  }

 private:
  std::string path_string;

 public:
#else  // #ifdef SCOREBOARD_APPLE_IMPL
// Enhanced alias to std::filesystem::path for non-Mac platforms.
class FilesystemPath : public std::filesystem::path {
 public:
  FilesystemPath() = default;
  explicit FilesystemPath(const std::string &str)
      : std::filesystem::path(str) {}

  static auto remove(const FilesystemPath &p) -> bool {
    return std::filesystem::remove(p);
  }

  static void rename(const FilesystemPath &a, const FilesystemPath &b) {
    std::filesystem::rename(a, b);
  }
#endif

  [[nodiscard]] auto existsWithRoot(const std::string &root) const -> bool;
  // Returns the name of a file with titlecase and best-guess spacing.
  [[nodiscard]] auto titleName() const -> std::string;
  static auto absolutePath(const std::string &root,
                           const std::string &file_path) -> std::string;
  static auto mostRelativePath(const std::string &root,
                               const std::string &file_path) -> std::string;

 private:
  static auto stripTrailingSeparator(const std::string &path) -> std::string;
};

}  // namespace cszb_scoreboard
