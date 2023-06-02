/*
util/FilesystemPath.cpp: In most cases, a moderately enhanced wrapper around
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

#include "util/FilesystemPath.h"

#include <stddef.h>  // for size_t

#include <algorithm>  // for count, transform
#include <array>      // for array
#include <cctype>     // for toupper
#include <sstream>    // for basic_istream, istringstream

#ifdef SCOREBOARD_APPLE_IMPL
#include <cstdio>  // for size_t, remove, rename
#endif

namespace cszb_scoreboard {

#ifdef SCOREBOARD_APPLE_IMPL

FilesystemPath::FilesystemPath() { path_string = ""; }

FilesystemPath::FilesystemPath(const std::string &str) { path_string = str; }

auto FilesystemPath::compare(const FilesystemPath &p) const -> int {
  return path_string.compare(p.path_string);
}

auto FilesystemPath::remove(const FilesystemPath &p) -> bool {
  if (p.path_string.empty()) {
    return false;
  }
  // cstdio remove returns 0 for success, so we return equality to 0 as a
  // success metric.
  return (std::remove(p.path_string.c_str()) == 0);
}

void FilesystemPath::rename(const FilesystemPath &a, const FilesystemPath &b) {
  std::rename(a.path_string.c_str(), b.path_string.c_str());
}

auto FilesystemPath::filename() const -> FilesystemPath {
  std::size_t separator = path_string.rfind('/');
  std::string filename = path_string;
  if (separator != std::string::npos) {
    filename = path_string.substr(separator + 1, path_string.size() - 1);
  }
  return FilesystemPath(filename);
}

auto FilesystemPath::pathname() const -> FilesystemPath {
  std::size_t separator = path_string.rfind('/');
  std::string pathname = path_string;
  if (separator != std::string::npos) {
    pathname = path_string.substr(0, separator);
  }
  return FilesystemPath(pathname);
}
auto FilesystemPath::replace_extension(const std::string &replacement) const
    -> FilesystemPath {
  std::string filename = path_string;
  std::size_t extension_start = filename.rfind('.');
  std::size_t separator = filename.rfind('/');
  // Remove the old extension.  Ignore dots in directories
  if (extension_start != std::string::npos &&
      (separator == std::string::npos || extension_start > separator)) {
    filename = path_string.substr(0, extension_start);
  }
  return FilesystemPath(filename + replacement);
}

void FilesystemPath::replace_filename(const std::string &new_filename) {
  path_string = pathname().string() + '/' + new_filename;
}

#endif  // #ifdef SCOREBOARD_APPLE_IMPL

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto FilesystemPath::existsWithRoot(const std::string &root) const -> bool {
#ifdef SCOREBOARD_APPLE_IMPL
  // TODO(#39): This is incorrect, but will need some testing on an actual MacOS
  // device when it's implemented.
  return true;
#else   // #ifdef SCOREBOARD_APPLE_IMPL
  return std::filesystem::exists(absolutePath(root, this->string()));
#endif  // #ifdef SCOREBOARD_APPLE_IMPL
}

auto FilesystemPath::stripTrailingSeparator(const std::string &path)
    -> std::string {
  if (path.length() > 0 && path.at(path.length() - 1) == preferred_separator) {
    return path.substr(0, path.length() - 1);
  }
  return path;
}

auto FilesystemPath::absolutePath(const std::string &root,
                                  const std::string &file_path) -> std::string {
  if (FilesystemPath(file_path).is_absolute()) {
    return file_path;
  }
  return stripTrailingSeparator(root) + static_cast<char>(preferred_separator) +
         file_path;
}

// If file_path is already relative, returns file_path.  If file_path is
// relative to root, returns the relative path.  Otherwise, returns file_path.
auto FilesystemPath::mostRelativePath(const std::string &root,
                                      const std::string &file_path)
    -> std::string {
  if (FilesystemPath(file_path).is_relative() || root.empty()) {
    return file_path;
  }
  auto effective_root = stripTrailingSeparator(root);
  // Search for root at the start of the string.
  if (file_path.rfind(effective_root + static_cast<char>(preferred_separator),
                      0) == 0) {
    return file_path.substr(effective_root.length() + 1, std::string::npos);
  }
  return file_path;
}

// List grabbed from the APA style guide -- it is likely not complete, but it's
// a good first attempt.
const std::array<const char *, 22> TITLE_EXCEPTIONS = {
    {"a",  "an",  "and", "as", "at",  "but", "by",  "for", "if", "in",  "nor",
     "of", "off", "on",  "or", "per", "so",  "the", "to",  "up", "via", "yet"}};

auto FilesystemPath::titleName() const -> std::string {
  std::string title = filename().replace_extension("").string();

  // Replace - and _ with spaces.
  std::transform(title.begin(), title.end(), title.begin(),
                 [](unsigned char c) {
                   if (c == '-' || c == '_') {
                     c = ' ';
                   }
                   return c;
                 });
  std::istringstream wordstream(title);
  title = "";
  std::string word;

  // Iterate through all words as a stream.
  while (wordstream >> word) {
    // If it's not the first word, add a space back in.
    if (!title.empty()) {
      title += " ";
    }
    // If it's the first word or not one of our exceptions, capitalize it.
    if (title.empty() || std::count(TITLE_EXCEPTIONS.begin(),
                                    TITLE_EXCEPTIONS.end(), word) == 0) {
      word[0] = std::toupper(word[0]);
    }
    title += word;
  }

  size_t last_space = title.find_last_of(' ');
  if (last_space > 0) {
    title[last_space + 1] = std::toupper(title[last_space + 1]);
  }

  return title;
}

}  // namespace cszb_scoreboard
