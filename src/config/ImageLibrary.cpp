/*
config/ImageLibrary.cpp: A singleton which represents a library of images,
tagged by the user and quickly searchable.

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

#include "config/ImageLibrary.h"

namespace cszb_scoreboard {

// Simple helper method which will insert a string into a sorted vector of
// strings, ignoring it if it's a duplicate.
void insertIntoSortedVector(std::vector<std::string> *vect,
                            const std::string &entry) {
  auto insertion_point = std::upper_bound(vect->begin(), vect->end(), entry);
  auto existing_element = std::lower_bound(vect->begin(), vect->end(), entry);
  if (existing_element == vect->end() || *existing_element != entry) {
    vect->insert(insertion_point, entry);
  }
}

ImageLibrary::ImageLibrary(proto::ImageLibrary library) {
  this->library = std::move(library);
}

auto ImageLibrary::getInstance() -> ImageLibrary * {
  static ImageLibrary singleton;
  return &singleton;
}

auto ImageLibrary::allFilenames() -> std::vector<FilesystemPath> {
  return search("").filenames();
}

auto ImageLibrary::allTags() -> std::vector<std::string> {
  std::vector<std::string> tags;
  for (const auto &image : library.images()) {
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
    if (image.file_path() == filename) {
      return image;
    }
  }
  return proto::ImageInfo();
}

auto ImageLibrary::name(const FilesystemPath &filename) -> std::string {
  return infoByFile(filename).name();
}

auto ImageLibrary::tags(const FilesystemPath &filename)
    -> std::vector<std::string> {
  proto::ImageInfo image = infoByFile(filename);
  std::vector<std::string> tags;
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
  Persistence::getInstance()->saveImageLibrary(library);
}

auto ImageLibrary::search(const std::string &query) -> ImageSearchResults {
  if (query.empty()) {
    // Empty search is a special case where we just return all
    return emptySearch();
  }

  auto tags = allTags();
  if (binary_search(tags.begin(), tags.end(), query)) {
    return exactMatchSearch(query);
  }
  return partialMatchSearch(query);
}

auto ImageLibrary::emptySearch() -> ImageSearchResults {
  std::vector<proto::ImageInfo> matched_images;
  for (const auto &image : library.images()) {
    matched_images.push_back(image);
  }
  return ImageSearchResults(matched_images, "", allTags());
}

auto ImageLibrary::exactMatchSearch(const std::string &query)
    -> ImageSearchResults {
  std::vector<proto::ImageInfo> matched_images;
  for (const auto &image : library.images()) {
    if (std::find(image.tags().begin(), image.tags().end(), query) !=
        image.tags().end()) {
      matched_images.push_back(image);
    }
  }
  if (matched_images.empty()) {
    return ImageSearchResults(matched_images, query,
                              std::vector<std::string>());
  }
  return ImageSearchResults(matched_images, query,
                            std::vector<std::string>({query}));
}

auto ImageLibrary::partialMatchSearch(const std::string &query)
    -> ImageSearchResults {
  std::vector<proto::ImageInfo> matched_images;
  std::vector<std::string> matched_tags;
  for (const auto &image : library.images()) {
    bool image_matched = false;
    for (const auto &tag : image.tags()) {
      if (std::search(tag.begin(), tag.end(), query.begin(), query.end()) !=
          tag.end()) {
        if (!image_matched) {
          matched_images.push_back(image);
        }
        image_matched = true;
        insertIntoSortedVector(&matched_tags, tag);
      }
    }
  }
  return ImageSearchResults(matched_images, query, matched_tags);
}

ImageSearchResults::ImageSearchResults(
    const std::vector<proto::ImageInfo> &matched_images,
    const std::string &search_string,
    const std::vector<std::string> &matched_tag_list) {
  this->matched_images = matched_images;
  this->search_string = search_string;
  this->matched_tag_list = matched_tag_list;
}

auto ImageSearchResults::filenames() -> std::vector<FilesystemPath> {
  std::vector<FilesystemPath> files;
  for (const auto &image : matched_images) {
    files.emplace_back(FilesystemPath(image.file_path()));
  }
  return files;
}

auto ImageSearchResults::matchedTags() -> std::vector<std::string> {
  std::vector<std::string> tags;
  for (const auto &tag : matched_tag_list) {
    tags.push_back(tag);
  }
  return tags;
}

}  // namespace cszb_scoreboard
