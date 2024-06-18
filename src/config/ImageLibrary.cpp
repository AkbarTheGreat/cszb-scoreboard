/*
config/ImageLibrary.cpp: A singleton which represents a library of images,
tagged by the user and quickly searchable.

Copyright 2020-2024 Tracy Beck

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

#include <algorithm>   // for max, binary_search, find
#include <cctype>      // for tolower
#include <compare>     // for operator<
#include <filesystem>  // for operator==, path
#include <utility>     // for move

#include "config/Persistence.h"   // for Persistence
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/Log.h"             // for LogDebug
#include "util/Singleton.h"       // for Singleton, SingletonClass
// IWYU pragma: no_include <google/protobuf/repeated_ptr_field.h>
// IWYU pragma: no_include "net/proto2/public/repeated_field.h"

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

// GCOVR_EXCL_START - This class uses our singleton objects.  In test, we
// always call the constructor that passes in the Singelton object, as it
// allows mocking of singletons.
ImageLibrary::ImageLibrary(SingletonClass c)
    : ImageLibrary(
          c, Singleton::getInstance(),
          Singleton::getInstance()->persistence()->loadImageLibrary()) {}
// GCOVR_EXCL_STOP

ImageLibrary::ImageLibrary(SingletonClass c, Singleton *singleton,
                           proto::ImageLibrary library) {
  this->singleton = singleton;
  this->library = std::move(library);
}

auto ImageLibrary::allFilenames() -> std::vector<FilesystemPath> {
  std::vector<FilesystemPath> filenames;
  filenames.reserve(library.images_size());
  for (const auto &image : library.images()) {
    filenames.emplace_back(image.file_path());
  }
  return filenames;
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

auto ImageLibrary::temporaryClone() -> std::unique_ptr<TemporaryImageLibrary> {
  proto::ImageLibrary library_copy;
  library_copy.CopyFrom(library);
  return std::make_unique<TemporaryImageLibrary>(singleton, library_copy);
}

void ImageLibrary::copyFrom(const TemporaryImageLibrary &other) {
  // Unlikely to ever change, but for completeness, re-assign the singleton
  // pointer.
  this->singleton = other.singleton;
  // Copy the incoming library to this one.
  this->library.CopyFrom(other.library);
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

void ImageLibrary::setName(const FilesystemPath &filename,
                           const std::string &name) {
  for (auto &image : *library.mutable_images()) {
    if (image.file_path() == filename.string()) {
      image.set_name(name);
      return;
    }
  }
  LogDebug("Attempt to set name of unknown file: %s", filename.c_str());
}

void ImageLibrary::setTags(const FilesystemPath &filename,
                           const std::vector<std::string> &tags) {
  for (auto &image : *library.mutable_images()) {
    if (image.file_path() == filename.string()) {
      image.clear_tags();
      for (const auto &tag : tags) {
        image.add_tags(tag);
      }
      return;
    }
  }
  LogDebug("Attempt to set tags of unknown file: %s", filename.c_str());
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
  FilesystemPath rel_path = FilesystemPath(
      FilesystemPath::mostRelativePath(libraryRoot().string(), file.string()));
  new_image->set_file_path(rel_path.string());
  new_image->set_is_relative(rel_path.is_relative());
  new_image->set_name(name);
  for (const auto &tag : tags) {
    new_image->add_tags(tag);
  }
}

void ImageLibrary::moveImage(const FilesystemPath &previous_path,
                             const FilesystemPath &new_path) {
  FilesystemPath rel_path = FilesystemPath(FilesystemPath::mostRelativePath(
      libraryRoot().string(), new_path.string()));
  auto *images = library.mutable_images();
  for (auto &image : *images) {
    if (FilesystemPath(image.file_path()) == previous_path) {
      image.set_file_path(rel_path.string());
      image.set_is_relative(rel_path.is_relative());
    }
  }
}

void ImageLibrary::deleteImage(const FilesystemPath &file) {
  auto *images = library.mutable_images();
  for (auto itr = images->begin(); itr < images->end(); itr++) {
    if (FilesystemPath(itr->file_path()) == file) {
      images->erase(itr);
      return;
    }
  }
}

auto ImageLibrary::libraryRoot() -> FilesystemPath {
  return FilesystemPath(library.library_root());
}

void ImageLibrary::removeLibraryRoot() {
  auto *images = library.mutable_images();
  for (auto &image : *images) {
    image.set_file_path(FilesystemPath::absolutePath(library.library_root(),
                                                     image.file_path()));
  }
  library.clear_library_root();
}

void ImageLibrary::moveLibraryRoot(const FilesystemPath &root) {
  library.set_library_root(root.string());
}

void ImageLibrary::setLibraryRoot(const FilesystemPath &root) {
  auto *images = library.mutable_images();
  for (auto &image : *images) {
    std::string abs_path =
        FilesystemPath::absolutePath(library.library_root(), image.file_path());
    std::string rel_path =
        FilesystemPath::mostRelativePath(root.string(), abs_path);
    image.set_file_path(rel_path);
    image.set_is_relative(FilesystemPath(rel_path).is_relative());
  }
  moveLibraryRoot(root);
}

void ImageLibrary::smartUpdateLibraryRoot(const FilesystemPath &root) {
  // First, make all paths absolute with the best possible option.
  auto *images = library.mutable_images();
  const std::string new_root = root.string();
  for (auto &image : *images) {
    if (image.is_relative()) {
      // * If only one path exists, use that one.
      // * If both paths exist, prefer the new path.
      // * If neither path is exists, leave the path as the original.
      // * Ultimately, this boils down to -- is new path valid?  Use that.
      // Otherwise, use the old one.
      if (FilesystemPath(image.file_path()).existsWithRoot(root.string())) {
        image.set_file_path(
            FilesystemPath::absolutePath(root.string(), image.file_path()));
      } else {
        image.set_file_path(FilesystemPath::absolutePath(library.library_root(),
                                                         image.file_path()));
      }
      image.set_is_relative(false);
    }
  }
  // Now, do a regular setLibraryRoot to establish new relative paths and update
  // the library root internally.
  setLibraryRoot(root);
}

void ImageLibrary::clearLibrary() { library.Clear(); }

void ImageLibrary::saveLibrary() {
  if (enable_persistence) {
    singleton->persistence()->saveImageLibrary(library);
  }
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

void ImageLibrary::addMatch(std::vector<proto::ImageInfo> *matched_images,
                            const proto::ImageInfo &image) {
  proto::ImageInfo image_copy(image);
  image_copy.set_file_path(
      FilesystemPath::absolutePath(library.library_root(), image.file_path()));
  matched_images->emplace_back(image_copy);
}

auto ImageLibrary::emptySearch() -> ImageSearchResults {
  std::vector<proto::ImageInfo> matched_images;
  for (const auto &image : library.images()) {
    addMatch(&matched_images, image);
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
      addMatch(&matched_images, image);
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
      addMatch(&matched_images, image);
      insertIntoSortedVector(&matched_tags, image.name());
      image_matched = true;
    }
    for (const auto &tag : image.tags()) {
      if (CaseOptionalString(tag).substring(lower_query)) {
        if (!image_matched) {
          addMatch(&matched_images, image);
        }
        image_matched = true;
        insertIntoSortedVector(&matched_tags, tag);
      }
    }
  }
  return {matched_images, query, matched_tags};
}

TemporaryImageLibrary::TemporaryImageLibrary(Singleton *singleton,
                                             proto::ImageLibrary library)
    : ImageLibrary(SingletonClass{}, singleton, std::move(library)) {
  enable_persistence = false;
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

auto CaseOptionalString::find(const CaseOptionalString &b,
                              size_t offset) const -> size_t {
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
