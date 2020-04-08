/*
config/ImageLibrary.h: A singleton which represents a library of images, tagged
by the user and quickly searchable.

Copyright 2020 Tracy Beck

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

#include "ScoreboardCommon.h"
#include "config/Persistence.h"
#include "image_library.pb.h"
#include "util/FilesystemPath.h"

namespace cszb_scoreboard {

class ImageSearchResults {
 public:
  std::vector<FilesystemPath> filenames();
  std::vector<std::string> matchedTags();

 private:
  ImageSearchResults(const std::vector<proto::ImageInfo> &matched_images,
                     const std::string &search_string,
                     const std::vector<std::string> &matched_tag_list);
  std::vector<proto::ImageInfo> matched_images;
  std::string search_string;
  std::vector<std::string> matched_tag_list;
  friend class ImageLibrary;
};

class ImageLibrary {
 public:
  static ImageLibrary *getInstance();
  // Returns all unique tags, sorted
  std::vector<FilesystemPath> allFilenames();
  std::vector<std::string> allTags();
  std::map<FilesystemPath, proto::ImageInfo> imageMap();
  std::string name(FilesystemPath filename);
  void addImage(FilesystemPath file, std::string name,
                              std::vector<std::string> tags);
  void clearLibrary();
  void saveLibrary();
  ImageSearchResults search(std::string query);
  std::vector<std::string> tags(FilesystemPath filename);

  PUBLIC_TEST_ONLY
  // Test-available constructor which initializes this object from an in-memory
  // proto.
  ImageLibrary(proto::ImageLibrary library);

 private:
  static ImageLibrary *singleton_instance;
  proto::ImageLibrary library;
  ImageLibrary()
      : ImageLibrary(Persistence::getInstance()->loadImageLibrary()) {}
  ImageSearchResults emptySearch();
  ImageSearchResults exactMatchSearch(std::string query);
  proto::ImageInfo infoByFile(FilesystemPath filename);
  ImageSearchResults partialMatchSearch(std::string query);
};

}  // namespace cszb_scoreboard
