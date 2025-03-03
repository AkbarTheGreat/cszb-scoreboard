/*
util/AutoUpdate.h: Singleton which handles checking for updates to the software.

Copyright 2019-2025 Tracy Beck

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
#include <utility>
#include <vector>

#include "HttpReader.h"
#include "ScoreboardCommon.h"
#include "util/Singleton.h"

namespace cszb_scoreboard {

class FilesystemPath;

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
  // GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
  // always call the constructor that passes in the Singleton object, as it
  // allows mocking of singletons.
  explicit AutoUpdate(SingletonClass c)
      : AutoUpdate(c, Singleton::getInstance(),
                   std::move(std::make_unique<HttpReader>())) {};
  // GCOVR_EXCL_STOP
  virtual auto checkForUpdate(const std::string &current_version) -> bool;
  [[nodiscard]] virtual auto updateIsDownloadable() const -> bool {
    return update_size > 0;
  }
  virtual auto downloadUpdate(std::vector<char> *update_data) -> bool;
  virtual void removeOldUpdate();
  virtual auto updateInPlace() -> bool;

  PUBLIC_TEST_ONLY
  virtual auto checkForUpdate(const std::string &current_version,
                              const std::string &platform_name) -> bool;
  AutoUpdate(SingletonClass c, Singleton *singleton,
             std::unique_ptr<HttpReader> reader);

 private:
  std::string new_binary_url;
  bool update_available;
  int update_size;
  std::unique_ptr<HttpReader> httpReader;
  Singleton *singleton;
  auto backupPath() -> FilesystemPath;
  auto downloadUpdate(const std::string &url, std::vector<char> *update_data)
      -> bool;
};

}  // namespace cszb_scoreboard
