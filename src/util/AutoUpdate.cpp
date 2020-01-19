/*
util/AutoUpdate.cpp: Singleton which handles checking for updates to the
software.

Copyright 2019 Tracy Beck

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

#include <vector>

#include "util/StringUtil.h"

namespace cszb_scoreboard {

// Here's the command line way to download the .exe.  Figure out how to make
// this work right in C++ curl - s
// https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases/latest |
// grep browser_download_url | cut -d : -f 2,3 | tr -d \" | wget -qi -

const char *LATEST_VERSION_URL =
    "https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases/"
    "latest";
AutoUpdate *AutoUpdate::singleton_instance;

AutoUpdate *AutoUpdate::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new AutoUpdate();
  }
  return singleton_instance;
}

size_t AutoUpdate::curlCallback(void *new_content, size_t byte_size,
                                size_t size, void *page_content) {
  size_t grow_size = byte_size * size;
  std::vector<char> *page_vector = (std::vector<char> *)page_content;

  if (page_vector->size() == 0) {
    page_vector->push_back('\0');
  }

  size_t old_size = page_vector->size();

  page_vector->resize(old_size + grow_size);

  memcpy(&(page_vector->data()[old_size - 1]), new_content, grow_size);

  (*page_vector)[page_vector->size() - 1] = '\0';

  return grow_size;
}

std::string AutoUpdate::checkForUpdate(const std::string current_version) {
  std::vector<char> raw_json;

  curl_global_init(CURL_GLOBAL_ALL);

  // setup curl junk
  CURL *curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, LATEST_VERSION_URL);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                   AutoUpdate::curlCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&raw_json);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  // Actual HTTP request
  CURLcode curl_response = curl_easy_perform(curl_handle);

  // cleanup curl junk
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  if (curl_response != CURLE_OK) {
    // Log an error, but otherwise ignore it, for user convenience.
    wxLogDebug("Curl failure checking for update: %s", curl_easy_strerror(curl_response));
    return "";
  }

  Json::Value root;
  std::string errors;
  Json::CharReader *json_reader = Json::CharReaderBuilder().newCharReader();
  json_reader->parse(raw_json.data(), raw_json.data() + raw_json.size(), &root,
                     &errors);

  Version new_version(root.get("name", current_version).asString());
  Version old_version(current_version);

  if (new_version > old_version) {
    Json::Value download_url = root["assets"][0]["browser_download_url"];
    Json::Value assets = root.get("assets", {});
    Json::Value asset_zero = assets.get((Json::Value::ArrayIndex)0, "");
    return asset_zero.get("browser_download_url", "").asString();
  }
  return "";
}

Version::Version(std::string version_string) {
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
      minor_component = StringUtil::stringToInt(
          version_string.substr(first_dot + 1, second_dot - first_dot - 1), 0);
      subminor_component = StringUtil::stringToInt(
          version_string.substr(second_dot + 1,
                                version_string.size() - second_dot - 1),
          0);
    }
  }
}

int Version::major() { return major_component; }
int Version::minor() { return minor_component; }
int Version::subminor() { return subminor_component; }

bool Version::operator==(const Version &b) const {
  return (this->major_component == b.major_component &&
          this->minor_component == b.minor_component &&
          this->subminor_component == b.subminor_component);
}
bool Version::operator!=(const Version &b) const { return !(*this == b); }

bool Version::operator<(const Version &b) const {
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
bool Version::operator>=(const Version &b) const { return !(*this < b); }

bool Version::operator>(const Version &b) const {
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
bool Version::operator<=(const Version &b) const { return !(*this > b); }

}  // namespace cszb_scoreboard
