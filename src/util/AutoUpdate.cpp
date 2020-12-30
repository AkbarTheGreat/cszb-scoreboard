/*
util/AutoUpdate.cpp: Singleton which handles checking for updates to the
software.

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

#include "util/AutoUpdate.h"

#include <curl/curl.h>
#include <json/json.h>
#include <json/reader.h>
#include <wx/wx.h>

#include <fstream>
#include <regex>

#include "config/CommandArgs.h"
#include "util/FilesystemPath.h"
#include "util/StringUtil.h"

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

const int MIN_HTTP_RESPONSE_SIZE = 50;
const int MAX_HTTP_RESPONSE_SIZE = 1000;
const int MAX_REDIRECT_ATTEMPTS = 5;

auto AutoUpdate::getInstance() -> AutoUpdate * {
  static AutoUpdate singleton;
  return &singleton;
}

auto curlCallback(void *new_content, size_t byte_size, size_t size,
                  void *page_content) -> size_t {
  size_t grow_size = byte_size * size;
  auto *page_vector = static_cast<std::vector<char> *>(page_content);

  if (page_vector->empty()) {
    page_vector->push_back('\0');
  }

  size_t old_size = page_vector->size();

  page_vector->resize(old_size + grow_size);

  // Call to data() is needed here to properly fill via memcpy.
  // NOLINTNEXTLINE(readability-simplify-subscript-expr)
  memcpy(&(page_vector->data()[old_size - 1]), new_content, grow_size);

  (*page_vector)[page_vector->size() - 1] = '\0';

  return grow_size;
}

auto curlRead(const char *url, std::vector<char> *response_buffer) -> CURLcode {
  curl_global_init(CURL_GLOBAL_ALL);

  // setup curl junk
  CURL *curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curlCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,
                   static_cast<void *>(response_buffer));
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

#ifdef __APPLE__
  curl_easy_setopt(curl_handle, CURLOPT_CAINFO, std::getenv("SSL_CERT_FILE"));
#endif

  // Actual HTTP request
  CURLcode curl_response = curl_easy_perform(curl_handle);

  // cleanup curl junk
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  return curl_response;
}

auto getHref(const std::string &html) -> std::string {
  const std::string href_prefix = "href=\"";
  size_t href_start = html.find(href_prefix, 0) + href_prefix.length();
  size_t href_end = html.find('"', href_start);

  if (href_start == -1 || href_end == -1) {
    return "";
  }

  std::string raw_href = html.substr(href_start, href_end - href_start);
  std::regex regex("&amp;");
  return std::regex_replace(raw_href, regex, "&");
}

// Returns "" if it's not a redirect
auto isRedirect(const std::vector<char> &http_data) -> std::string {
  if (http_data.size() > MAX_HTTP_RESPONSE_SIZE ||
      http_data.size() < MIN_HTTP_RESPONSE_SIZE) {
    return "";
  }
  // Check to see if it looks like an html page.
  if (http_data[0] != '<' || http_data[1] != 'h' || http_data[2] != 't' ||
      http_data[3] != 'm' || http_data[4] != 'l' ||
      http_data[5] != '>') {  // NOLINT(readability-magic-numbers) 5 is not
                              // really magic here.
    return "";
  }
  return getHref(http_data.data());
}

auto backupPath() -> FilesystemPath {
  FilesystemPath backup_path = CommandArgs::getInstance()->commandPath();
  backup_path.replace_filename(AUTO_UPDATE_BACKUP_NAME);
  return backup_path;
}

auto AutoUpdate::checkForUpdate(const std::string &current_version) -> bool {
  return checkForUpdate(current_version, AUTO_UPDATE_PLATFORM_NAME);
}

auto AutoUpdate::checkForUpdate(const std::string &current_version,
                                const std::string &platform_name) -> bool {
  std::vector<char> raw_json;

  CURLcode curl_response = curlRead(LATEST_VERSION_URL, &raw_json);
  if (curl_response != CURLE_OK) {
    // Log an error, but otherwise ignore it, for user convenience.
    wxLogDebug("Curl failure checking for update: %s",
               curl_easy_strerror(curl_response));
    return false;
  }

  Json::Value root;
  std::string errors;
  Json::CharReader *json_reader = Json::CharReaderBuilder().newCharReader();
  json_reader->parse(raw_json.data(), raw_json.data() + raw_json.size(), &root,
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
                                std::vector<char> *update_data,
                                int redirect_depth) -> bool {
  if (redirect_depth > MAX_REDIRECT_ATTEMPTS) {
    wxLogDebug("Too many redirects.  Cancelling update.");
    return false;
  }

  CURLcode curl_response = curlRead(url.c_str(), update_data);
  if (curl_response != CURLE_OK) {
    // Log an error, but otherwise ignore it, for user convenience.
    wxLogDebug("Curl failure checking for update: %s",
               curl_easy_strerror(curl_response));
    return false;
  }

  std::string redirect = isRedirect(*update_data);

  if (!redirect.empty()) {
    update_data->resize(0);
    return downloadUpdate(redirect, update_data, ++redirect_depth);
  }

  // Presuming that this is binary, we've added a null at the end we now need to
  // shed.
  if (update_data->size() > 1) {
    update_data->resize(update_data->size() - 1);
  }

  if (update_data->size() != update_size) {
    wxLogDebug("Problem with update!  Expected %d bytes, but got %d.",
               update_size, static_cast<int>(update_data->size()));
    wxLogDebug("Data: %s.", update_data->data());
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
  wxLogDebug("Writing auto-update to %s",
             CommandArgs::getInstance()->commandPath().string());

  FilesystemPath executable_path = CommandArgs::getInstance()->commandPath();
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
