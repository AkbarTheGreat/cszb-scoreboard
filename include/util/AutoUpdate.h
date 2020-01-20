/*
util/AutoUpdate.h: Singleton which handles checking for updates to the software.

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

// Here's the command line way to download the .exe.  Figure out how to make
// this work right in C++ curl - s
// https://api.github.com/repos/AkbarTheGreat/cszb-scoreboard/releases/latest |
// grep browser_download_url | cut -d : -f 2,3 | tr -d \" | wget -qi -

#pragma once

#include <string>

namespace cszb_scoreboard {

class Version {
 public:
  Version(std::string version_string);
  int major();
  int minor();
  int subminor();

  bool operator==(const Version &b) const;
  bool operator!=(const Version &b) const;
  bool operator<(const Version &b) const;
  bool operator>(const Version &b) const;
  bool operator<=(const Version &b) const;
  bool operator>=(const Version &b) const;

 private:
  int major_component, minor_component, subminor_component;
};

class AutoUpdate {
 public:
  static AutoUpdate *getInstance();
  std::string checkForUpdate(const std::string current_version);

 private:
  static AutoUpdate *singleton_instance;
  static size_t curlCallback(void *page_content, size_t byte_size, size_t size,
                             void *userp);
};

}  // namespace cszb_scoreboard