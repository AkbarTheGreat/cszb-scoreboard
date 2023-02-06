/*
config/ImageLibrary.h: A singleton which represents a library of images, tagged
by the user and quickly searchable.

Copyright 2020-2023 Tracy Beck

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
#include <memory>   // for unique_ptr
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

class TemporaryImageLibrary;

class ImageLibrary {
 public:
  explicit ImageLibrary(SingletonClass c);
  auto allFilenames() -> std::vector<FilesystemPath>;
  // Returns all unique tags, sorted
  auto allTags(bool include_name = false) const
      -> std::vector<CaseOptionalString>;
  auto temporaryClone() -> std::unique_ptr<TemporaryImageLibrary>;
  void copyFrom(const TemporaryImageLibrary &other);
  auto name(const FilesystemPath &filename) -> std::string;
  void setName(const FilesystemPath &filename, const std::string &name);

  void addImage(const FilesystemPath &file, const std::string &name,
                const std::vector<std::string> &tags);
  void moveImage(const FilesystemPath &previous_path,
                 const FilesystemPath &new_path);
  void deleteImage(const FilesystemPath &file);
  auto libraryRoot() -> FilesystemPath;
  void removeLibraryRoot();
  // moveLibraryRoot moves the root without changing any relative paths.  Use
  // this if the entire directory was relocated and you want to shift them all
  // at one time.
  void moveLibraryRoot(const FilesystemPath &root);
  // setLibraryRoot sets the root, updating every file to match the new relative
  // location.  Use this if the root is changed, but no files are moved.
  void setLibraryRoot(const FilesystemPath &root);
  // smartUpdateLibraryRoot sets the root, and for relative paths in the library
  // makes a best effort to set their location in a way that makes sense. Use
  // this for most user operations where the root changes.
  void smartUpdateLibraryRoot(const FilesystemPath &root);
  void clearLibrary();
  void saveLibrary();
  auto search(const std::string &query) -> ImageSearchResults;
  auto tags(const FilesystemPath &filename) -> std::vector<CaseOptionalString>;
  void setTags(const FilesystemPath &filename,
               const std::vector<std::string> &tags);

  PUBLIC_TEST_ONLY
  // Test-available constructor which initializes this object from an in-memory
  // proto.
  ImageLibrary(SingletonClass c, Singleton *singleton,
               proto::ImageLibrary library);

 protected:
  bool enable_persistence = true;

 private:
  proto::ImageLibrary library;
  Singleton *singleton;
  auto emptySearch() -> ImageSearchResults;
  auto exactMatchSearch(const std::string &query) -> ImageSearchResults;
  auto infoByFile(const FilesystemPath &filename) -> proto::ImageInfo;
  auto partialMatchSearch(const std::string &query) -> ImageSearchResults;
  void addMatch(std::vector<proto::ImageInfo> *matched_images,
                const proto::ImageInfo &image);
};

// A non-singleton subclass of the singleton ImageLibrary which turns off all
// persistence functionality in the ImageLibrary, but otherwise is editable.
class TemporaryImageLibrary : public ImageLibrary {
 public:
  TemporaryImageLibrary(Singleton *singleton, proto::ImageLibrary library);
};

}  // namespace cszb_scoreboard
