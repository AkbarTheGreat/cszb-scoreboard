/*
config/ImageLibrary.cpp: A singleton which represents a library of images,
tagged by the user and quickly searchable.

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

#include "config/ImageLibrary.h"

#include <algorithm>   // for binary_search, find, lower_bound
#include <cctype>      // for tolower
#include <filesystem>  // for operator<
#include <utility>     // for move

#include "config/Persistence.h"   // for Persistence
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/Singleton.h"       // for Singleton, SingletonClass

namespace cszb_scoreboard {

// Simple helper method which will insert a string into a sorted vector of
// strings, ignoring it if it's a duplicate.
void insertIntoSortedVector(std::vector<CaseOptionalString> *vect,
                            const std ::string &entry) {
  auto wrapped_entry = CaseOptionalString(entry);
  auto existing_element =
      std::lower_bound(vect->begin(), vect->end(), wrapped_entry);
  if (existing_element == vect->end() || *existing_element != wrapped_entry) {
    auto insertion_point =
        std::upper_bound(vect->begin(), vect->end(), wrapped_entry);
    vect->insert(insertion_point, wrapped_entry);
  }
}

ImageLibrary::ImageLibrary(SingletonClass c)
    : ImageLibrary(
          c, Singleton::getInstance(),
          Singleton::getInstance()->persistence()->loadImageLibrary()) {}

ImageLibrary::ImageLibrary(SingletonClass c, Singleton *singleton,
                           proto::ImageLibrary library) {
  this->singleton = singleton;
  this->library = std::move(library);
}

auto ImageLibrary::allFilenames() -> std::vector<FilesystemPath> {
  return search("").filenames();
}

auto ImageLibrary::allTags(bool include_name) const
    -> std::vector<CaseOptionalString> {
  std::vector<CaseOptionalString> tags;
  for (const auto &image : library.images()) {
    if (include_name) {
      insertIntoSortedVector(&tags, image.name());
    }
    for (const auto &tag : image.tags()) {
      insertIntoSortedVector(&tags, tag);
    }
  }
  return tags;
}

auto ImageLibrary::imageMap() -> std::map<FilesystemPath, proto::ImageInfo> {
  std::map<FilesystemPath, proto::ImageInfo> image_map;
  for (const auto &image : library.images()) {
    image_map.emplace(FilesystemPath(image.file_path()), image);
  }
  return image_map;
}

auto ImageLibrary::infoByFile(const FilesystemPath &filename)
    -> proto::ImageInfo {
  for (auto image : library.images()) {
    if (image.file_path() == filename.string()) {
      return image;
    }
  }
  return {};
}

auto ImageLibrary::name(const FilesystemPath &filename) -> std::string {
  return infoByFile(filename).name();
}

auto ImageLibrary::tags(const FilesystemPath &filename)
    -> std::vector<CaseOptionalString> {
  proto::ImageInfo image = infoByFile(filename);
  std::vector<CaseOptionalString> tags;
  for (const auto &tag : image.tags()) {
    insertIntoSortedVector(&tags, tag);
  }
  return tags;
}

void ImageLibrary::addImage(const FilesystemPath &file, const std::string &name,
                            const std::vector<std::string> &tags) {
  proto::ImageInfo *new_image = library.add_images();
  new_image->set_file_path(file.string());
  new_image->set_name(name);
  for (const auto &tag : tags) {
    new_image->add_tags(tag);
  }
}

void ImageLibrary::clearLibrary() { library.Clear(); }

void ImageLibrary::saveLibrary() {
  singleton->persistence()->saveImageLibrary(library);
}

auto ImageLibrary::search(const std::string &query) -> ImageSearchResults {
  if (query.empty()) {
    // Empty search is a special case where we just return all
    return emptySearch();
  }

  auto tags = allTags(/*include_name=*/true);
  if (binary_search(tags.begin(), tags.end(), CaseOptionalString(query))) {
    return exactMatchSearch(query);
  }
  return partialMatchSearch(query);
}

auto ImageLibrary::emptySearch() -> ImageSearchResults {
  std::vector<proto::ImageInfo> matched_images;
  for (const auto &image : library.images()) {
    matched_images.push_back(image);
  }
  return {matched_images, "", allTags()};
}

auto ImageLibrary::exactMatchSearch(const std::string &query)
    -> ImageSearchResults {
  std::vector<proto::ImageInfo> matched_images;
  for (const auto &image : library.images()) {
    if ((image.name() == query) ||
        (std::find(image.tags().begin(), image.tags().end(), query) !=
         image.tags().end())) {
      matched_images.push_back(image);
    }
  }
  if (matched_images.empty()) {
    return {matched_images, query, std::vector<CaseOptionalString>()};
  }
  return {matched_images, query,
          std::vector<CaseOptionalString>({CaseOptionalString(query)})};
}

auto ImageLibrary::partialMatchSearch(const std::string &query)
    -> ImageSearchResults {
  CaseOptionalString lower_query(query);
  std::vector<proto::ImageInfo> matched_images;
  std::vector<CaseOptionalString> matched_tags;
  for (const auto &image : library.images()) {
    bool image_matched = false;
    if (CaseOptionalString(image.name()).find(lower_query) !=
        std::string::npos) {
      matched_images.push_back(image);
      insertIntoSortedVector(&matched_tags, image.name());
      image_matched = true;
    }
    for (const auto &tag : image.tags()) {
      if (CaseOptionalString(tag).substring(lower_query)) {
        if (!image_matched) {
          matched_images.push_back(image);
        }
        image_matched = true;
        insertIntoSortedVector(&matched_tags, tag);
      }
    }
  }
  return {matched_images, query, matched_tags};
}

ImageSearchResults::ImageSearchResults(
    const std::vector<proto::ImageInfo> &matched_images,
    const std::string &search_string,
    const std::vector<CaseOptionalString> &matched_tag_list) {
  this->matched_images = matched_images;
  this->search_string = search_string;
  this->matched_tag_list = matched_tag_list;
}

auto ImageSearchResults::filenames() -> std::vector<FilesystemPath> {
  std::vector<FilesystemPath> files;
  files.reserve(matched_images.size());
  for (const auto &image : matched_images) {
    files.emplace_back(image.file_path());
  }
  return files;
}

auto ImageSearchResults::matchedTags() -> std::vector<std::string> {
  std::vector<std::string> tags;
  tags.reserve(matched_tag_list.size());
  for (const auto &tag : matched_tag_list) {
    tags.push_back(tag.string());
  }
  return tags;
}

CaseOptionalString::CaseOptionalString(const std::string &str) {
  value = str;
  lowercase = str;
  std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}

auto CaseOptionalString::find(const CaseOptionalString &b, size_t offset) const
    -> size_t {
  return lowercase.find(b.lowercase, offset);
}

auto CaseOptionalString::substring(const CaseOptionalString &b) const -> bool {
  return std::search(lowercase.begin(), lowercase.end(), b.lowercase.begin(),
                     b.lowercase.end()) != lowercase.end();
}

auto CaseOptionalString::compare(const CaseOptionalString &b) const noexcept
    -> int {
  if (value == b.value) {
    return 0;
  }
  if (lowercase == b.lowercase) {
    if (value < b.value) {
      return -1;
    }
    return 1;
  }
  if (lowercase < b.lowercase) {
    return -1;
  }
  return 1;
}

auto CaseOptionalString::operator==(const CaseOptionalString &b) const noexcept
    -> bool {
  return this->compare(b) == 0;
}

auto CaseOptionalString::operator!=(const CaseOptionalString &b) const noexcept
    -> bool {
  return !(*this == b);
}

auto CaseOptionalString::operator<(const CaseOptionalString &b) const noexcept
    -> bool {
  return this->compare(b) < 0;
}

auto CaseOptionalString::operator<=(const CaseOptionalString &b) const noexcept
    -> bool {
  return (*this == b || *this < b);
}

auto CaseOptionalString::operator>(const CaseOptionalString &b) const noexcept
    -> bool {
  return !(*this <= b);
}

auto CaseOptionalString::operator>=(const CaseOptionalString &b) const noexcept
    -> bool {
  return !(*this < b);
}

}  // namespace cszb_scoreboard
