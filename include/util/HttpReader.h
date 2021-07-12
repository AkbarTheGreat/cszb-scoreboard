/*
util/HttpReader.h: Simple class which uses Curl to pull data over HTTP.

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

#include <curl/curl.h>
#include <string>
#include <vector>

namespace cszb_scoreboard {

struct HttpResponse {
  // Empty string indicates that there was no error.
  std::string error;
  std::vector<char> response;
};

class HttpReader {
 public:
  virtual ~HttpReader() = default;
  virtual auto read(const char *url) -> HttpResponse;
};

}  // namespace cszb_scoreboard
