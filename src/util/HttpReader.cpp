/*
util/HttpReader.h: Simple class which uses Curl to pull data over HTTP.

Copyright 2019-2022 Tracy Beck

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

#include <cstring>
#include <regex>  // for match_results<>::_Base_type, regex_...

#include "util/Log.h"  // for LogDebug

namespace cszb_scoreboard {

constexpr int MAX_REDIRECT_ATTEMPTS = 5;
constexpr int MIN_HTTP_RESPONSE_SIZE = 50;
constexpr int MAX_HTTP_RESPONSE_SIZE = 1000;

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
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);

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

auto HttpReader::readBinary(const char *url, std::vector<char> *bin_data,
                            int redirect_depth) -> bool {
  if (redirect_depth > MAX_REDIRECT_ATTEMPTS) {
    LogDebug("Too many redirects.  Cancelling update.");
    return false;
  }

  HttpResponse http_response = read(url);
  if (!http_response.error.empty()) {
    // Log an error, but otherwise ignore it, for user convenience.
    LogDebug("Curl failure checking for update: %s",
             http_response.error.c_str());
    return false;
  }

  std::string redirect = isRedirect(http_response.response);

  if (!redirect.empty()) {
    bin_data->resize(0);
    return readBinary(redirect.c_str(), bin_data, ++redirect_depth);
  }

  *bin_data = http_response.response;

  // Since we're assuming that this is binary, we've added a null at the end we
  // now need to shed.
  if (bin_data->size() > 1) {
    bin_data->resize(bin_data->size() - 1);
  }

  return true;
}

}  // namespace cszb_scoreboard
