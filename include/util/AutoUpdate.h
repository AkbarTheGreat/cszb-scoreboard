/*
util/AutoUpdate.h: Singleton which handles checking for updates to the software.

Copyright 2019-2021 Tracy Beck

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

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "HttpReader.h"
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
  explicit AutoUpdate(std::unique_ptr<HttpReader> reader) {
    httpReader = std::move(reader);
  }

 private:
  std::string new_binary_url;
  bool update_available;
  int update_size;
  std::unique_ptr<HttpReader> httpReader;
  AutoUpdate() : AutoUpdate(std::move(std::make_unique<HttpReader>())){};
  auto downloadUpdate(const std::string &url, std::vector<char> *update_data,
                      int redirect_depth = 0) -> bool;
};

}  // namespace cszb_scoreboard
