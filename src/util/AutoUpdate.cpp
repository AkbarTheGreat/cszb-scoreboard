/*
util/AutoUpdate.cpp: Singleton which handles checking for updates to the
software.

Copyright 2019-2024 Tracy Beck

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

#include "util/AutoUpdate.h"

#include <json/reader.h>  // for CharReaderBuilder, CharReader
#include <json/value.h>   // for Value, ValueIterator, ValueIterator...

#include <cstddef>  // for size_t
#include <fstream>  // for operator|, fstream, ios, basic_ostr...

#include "config/CommandArgs.h"   // for CommandArgs
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/HttpReader.h"      // for HttpResponse, HttpReader
#include "util/Log.h"             // for LogDebug
#include "util/StringUtil.h"      // for StringUtil

namespace cszb_scoreboard {

#ifdef _WIN32
const char *AUTO_UPDATE_PLATFORM_NAME = "Win64";
#else  // ifdef _WIN32
#ifdef __APPLE__
// Autoupdate disabled for MacOS for now, some work needs to happen to download
// and unzip the release, which is more complicated than a Windows install. Once
// the mechanism is in place, remove -autoupdatedisabled- below.
const char *AUTO_UPDATE_PLATFORM_NAME = "MacOS-autoupdatedisabled-";
#else   // ifdef __APPLE__
const char *AUTO_UPDATE_PLATFORM_NAME = "Unknown";
#endif  // ifdef __APPLE__
#endif  // ifdef _WIN32

const char *AUTO_UPDATE_BACKUP_NAME = "old_version_to_be_deleted";

const char *LATEST_VERSION_URL =
    "https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases/"
    "latest";

AutoUpdate::AutoUpdate(SingletonClass c, Singleton *singleton,
                       std::unique_ptr<HttpReader> reader) {
  this->singleton = singleton;
  httpReader = std::move(reader);
}

auto AutoUpdate::backupPath() -> FilesystemPath {
  FilesystemPath backup_path = singleton->commandArgs()->commandPath();
  backup_path.replace_filename(AUTO_UPDATE_BACKUP_NAME);
  return backup_path;
}

auto AutoUpdate::checkForUpdate(const std::string &current_version) -> bool {
  return checkForUpdate(current_version, AUTO_UPDATE_PLATFORM_NAME);
}

auto AutoUpdate::checkForUpdate(const std::string &current_version,
                                const std::string &platform_name) -> bool {
  HttpResponse http_response = httpReader->read(LATEST_VERSION_URL);
  if (!http_response.error.empty()) {
    // Log an error, but otherwise ignore it, for user convenience.
    LogDebug("Curl failure checking for update: %s",
             http_response.error.c_str());
    return false;
  }

  Json::Value root;
  std::string errors;
  Json::CharReader *json_reader = Json::CharReaderBuilder().newCharReader();
  json_reader->parse(
      http_response.response.data(),
      http_response.response.data() + http_response.response.size(), &root,
      &errors);

  Version new_version(root.get("name", current_version).asString());
  Version old_version(current_version);

  update_available = false;
  if (new_version > old_version) {
    Json::Value assets = root.get("assets", {});
    for (const auto &asset : assets) {
      std::string asset_platform_name = asset.get("label", "").asString();
      update_size = 0;
      update_available = true;
      if (asset_platform_name == platform_name) {
        update_size = asset.get("size", "0").asInt();
        new_binary_url = asset.get("browser_download_url", "").asString();
        break;
      }
    }
  }
  return update_available;
}

auto AutoUpdate::downloadUpdate(const std::string &url,
                                std::vector<char> *update_data) -> bool {
  if (!httpReader->readBinary(url.c_str(), update_data)) {
    return false;
  }

  if (update_data->size() != update_size) {
    LogDebug("Problem with update!  Expected %d bytes, but got %d.",
             update_size, static_cast<int>(update_data->size()));
    LogDebug("Data: %s.", update_data->data());
    return false;
  }

  return true;
}

auto AutoUpdate::downloadUpdate(std::vector<char> *update_data) -> bool {
  return downloadUpdate(new_binary_url, update_data);
}

auto AutoUpdate::updateInPlace() -> bool {
  std::vector<char> data_response;

  if (!downloadUpdate(&data_response)) {
    return false;
  }
  LogDebug("Writing auto-update to %s",
           singleton->commandArgs()->commandPath().c_str());

  FilesystemPath executable_path = singleton->commandArgs()->commandPath();
  FilesystemPath backup_path = backupPath();

  FilesystemPath::rename(executable_path, backup_path);

  std::fstream output(executable_path.c_str(),
                      std::ios::out | std::ios::trunc | std::ios::binary);

  output.write(data_response.data(), data_response.size());
  output.close();
  return true;
}

void AutoUpdate::removeOldUpdate() { FilesystemPath::remove(backupPath()); }

Version::Version(const std::string &version_string) {
  size_t first_dot = version_string.find('.', 0);
  size_t second_dot = version_string.find('.', first_dot + 1);
  major_component = minor_component = subminor_component = 0;
  if (first_dot == -1) {
    major_component = StringUtil::stringToInt(version_string, 0);
  } else {
    major_component =
        StringUtil::stringToInt(version_string.substr(0, first_dot), 0);
    if (second_dot == -1) {
      minor_component = StringUtil::stringToInt(
          version_string.substr(first_dot + 1,
                                version_string.size() - first_dot - 1),
          0);
    } else {
      size_t end_point = version_string.find('_', second_dot + 1);
      if (end_point == -1) {
        end_point = version_string.size();
      }

      minor_component = StringUtil::stringToInt(
          version_string.substr(first_dot + 1, second_dot - first_dot - 1), 0);

      subminor_component = StringUtil::stringToInt(
          version_string.substr(second_dot + 1, end_point - second_dot - 1), 0);
    }
  }
}

auto Version::operator==(const Version &b) const -> bool {
  return (this->major_component == b.major_component &&
          this->minor_component == b.minor_component &&
          this->subminor_component == b.subminor_component);
}

auto Version::operator!=(const Version &b) const -> bool {
  return !(*this == b);
}

auto Version::operator<(const Version &b) const -> bool {
  if (this->major_component > b.major_component) {
    return false;
  }
  if (this->major_component < b.major_component) {
    return true;
  }
  if (this->minor_component > b.minor_component) {
    return false;
  }
  if (this->minor_component < b.minor_component) {
    return true;
  }
  if (this->subminor_component < b.subminor_component) {
    return true;
  }
  return false;
}

auto Version::operator>=(const Version &b) const -> bool {
  return !(*this < b);
}

auto Version::operator>(const Version &b) const -> bool {
  if (this->major_component < b.major_component) {
    return false;
  }
  if (this->major_component > b.major_component) {
    return true;
  }
  if (this->minor_component < b.minor_component) {
    return false;
  }
  if (this->minor_component > b.minor_component) {
    return true;
  }
  if (this->subminor_component > b.subminor_component) {
    return true;
  }
  return false;
}

auto Version::operator<=(const Version &b) const -> bool {
  return !(*this > b);
}

}  // namespace cszb_scoreboard
