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

#include "util/HttpReader.h"

#include <curl/curl.h>
#include <string.h>

namespace cszb_scoreboard {

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

auto HttpReader::read(const char *url) -> HttpResponse {
  HttpResponse http_response;
  curl_global_init(CURL_GLOBAL_ALL);

  // setup curl junk
  CURL *curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curlCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,
                   static_cast<void *>(&http_response.response));
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

#ifdef __APPLE__
  curl_easy_setopt(curl_handle, CURLOPT_CAINFO, std::getenv("SSL_CERT_FILE"));
#endif

  // Actual HTTP request
  CURLcode curl_response = curl_easy_perform(curl_handle);

  // cleanup curl junk
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  if (curl_response != CURLE_OK) {
    http_response.error = curl_easy_strerror(curl_response);
    return http_response;
  }

  return http_response;
}

}  // namespace cszb_scoreboard
