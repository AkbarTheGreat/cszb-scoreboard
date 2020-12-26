/*
util/AutoUpdate.h: Singleton which handles checking for updates to the software.

Copyright 2019-2020 Tracy Beck

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

// Here's the command line way to download the .exe.  Figure out how to make
// this work right in C++ curl - s
// https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases/latest |
// grep browser_download_url | cut -d : -f 2,3 | tr -d \" | wget -qi -

#pragma once

#include <string>
#include <vector>

#include "ScoreboardCommon.h"

namespace cszb_scoreboard {

class Version {
 public:
  explicit Version(const std::string &version_string);
  [[nodiscard]] auto major() const -> int { return major_component; }
  [[nodiscard]] auto minor() const -> int { return minor_component; }
  [[nodiscard]] auto subminor() const -> int { return subminor_component; }

  auto operator==(const Version &b) const -> bool;
  auto operator!=(const Version &b) const -> bool;
  auto operator<(const Version &b) const -> bool;
  auto operator>(const Version &b) const -> bool;
  auto operator<=(const Version &b) const -> bool;
  auto operator>=(const Version &b) const -> bool;

 private:
  int major_component, minor_component, subminor_component;
};

class AutoUpdate {
 public:
  static auto getInstance() -> AutoUpdate *;
  auto checkForUpdate(const std::string &current_version) -> bool;
  [[nodiscard]] auto updateIsDownloadable() const -> bool {
    return update_size > 0;
  }
  auto downloadUpdate(std::vector<char> *update_data) -> bool;
  static void removeOldUpdate();
  auto updateInPlace() -> bool;

  PUBLIC_TEST_ONLY
  auto checkForUpdate(const std::string &current_version,
                      const std::string &platform_name) -> bool;

 private:
  std::string new_binary_url;
  bool update_available;
  int update_size;
  AutoUpdate() = default;
  auto downloadUpdate(const std::string &url, std::vector<char> *update_data,
                      int redirect_depth = 0) -> bool;
};

}  // namespace cszb_scoreboard
