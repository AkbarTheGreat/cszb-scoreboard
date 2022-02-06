/*
config/ImageLibrary.h: A singleton which represents a library of images, tagged
by the user and quickly searchable.

Copyright 2020-2022 Tracy Beck

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

#include <cstddef>  // for size_t
#include <map>      // for map
#include <string>   // for string, basic_string
#include <vector>   // for vector

#include "ScoreboardCommon.h"  // for PUBLIC_TEST_ONLY
#include "image_library.pb.h"  // for ImageInfo, ImageLibrary

namespace cszb_scoreboard {
class FilesystemPath;
class Singleton;
struct SingletonClass;

class CaseOptionalString {
 public:
  explicit CaseOptionalString(const std::string &str);
  [[nodiscard]] auto lower() const -> std::string { return lowercase; }
  [[nodiscard]] auto string() const -> std::string { return value; }
  [[nodiscard]] auto find(const CaseOptionalString &b, size_t offset = 0) const
      -> size_t;
  [[nodiscard]] auto substring(const CaseOptionalString &b) const -> bool;
  [[nodiscard]] auto compare(const CaseOptionalString &b) const noexcept -> int;
  auto operator==(const CaseOptionalString &b) const noexcept -> bool;
  auto operator!=(const CaseOptionalString &b) const noexcept -> bool;
  auto operator<(const CaseOptionalString &b) const noexcept -> bool;
  auto operator<=(const CaseOptionalString &b) const noexcept -> bool;
  auto operator>(const CaseOptionalString &b) const noexcept -> bool;
  auto operator>=(const CaseOptionalString &b) const noexcept -> bool;

 private:
  std::string value;
  std::string lowercase;
};

class ImageSearchResults {
 public:
  auto filenames() -> std::vector<FilesystemPath>;
  auto matchedTags() -> std::vector<std::string>;

 private:
  ImageSearchResults(const std::vector<proto::ImageInfo> &matched_images,
                     const std::string &search_string,
                     const std::vector<CaseOptionalString> &matched_tag_list);
  std::vector<proto::ImageInfo> matched_images;
  std::string search_string;
  std::vector<CaseOptionalString> matched_tag_list;
  friend class ImageLibrary;
};

class ImageLibrary {
 public:
  explicit ImageLibrary(SingletonClass c);
  // Returns all unique tags, sorted
  auto allFilenames() -> std::vector<FilesystemPath>;
  auto allTags(bool include_name = false) const
      -> std::vector<CaseOptionalString>;
  auto imageMap() -> std::map<FilesystemPath, proto::ImageInfo>;
  auto name(const FilesystemPath &filename) -> std::string;
  void addImage(const FilesystemPath &file, const std::string &name,
                const std::vector<std::string> &tags);
  void clearLibrary();
  void saveLibrary();
  auto search(const std::string &query) -> ImageSearchResults;
  auto tags(const FilesystemPath &filename) -> std::vector<CaseOptionalString>;

  PUBLIC_TEST_ONLY
  // Test-available constructor which initializes this object from an in-memory
  // proto.
  ImageLibrary(SingletonClass c, Singleton *singleton,
               proto::ImageLibrary library);

 private:
  proto::ImageLibrary library;
  Singleton *singleton;
  auto emptySearch() -> ImageSearchResults;
  auto exactMatchSearch(const std::string &query) -> ImageSearchResults;
  auto infoByFile(const FilesystemPath &filename) -> proto::ImageInfo;
  auto partialMatchSearch(const std::string &query) -> ImageSearchResults;
};

}  // namespace cszb_scoreboard
