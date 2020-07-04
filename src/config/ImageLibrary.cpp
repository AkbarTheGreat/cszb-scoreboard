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

ImageLibrary *ImageLibrary::singleton_instance = nullptr;

// Simple helper method which will insert a string into a sorted vector of
// strings, ignoring it if it's a duplicate.
void insertIntoSortedVector(std::vector<std::string> &vect, std::string entry) {
  auto insertion_point = std::upper_bound(vect.begin(), vect.end(), entry);
  auto existing_element = std::lower_bound(vect.begin(), vect.end(), entry);
  if (existing_element == vect.end() || *existing_element != entry) {
    vect.insert(insertion_point, entry);
  }
}

ImageLibrary::ImageLibrary(proto::ImageLibrary library) {
  this->library = library;
}

ImageLibrary *ImageLibrary::getInstance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new ImageLibrary();
    singleton_instance->library =
        Persistence::getInstance()->loadImageLibrary();
  }
  return singleton_instance;
}

std::vector<FilesystemPath> ImageLibrary::allFilenames() {
  return search("").filenames();
}

std::vector<std::string> ImageLibrary::allTags() {
  std::vector<std::string> tags;
  for (auto image : library.images()) {
    for (auto tag : image.tags()) {
      insertIntoSortedVector(tags, tag);
    }
  }
  return tags;
}

std::map<FilesystemPath, proto::ImageInfo> ImageLibrary::imageMap() {
  std::map<FilesystemPath, proto::ImageInfo> image_map;
  for (auto image : library.images()) {
    image_map.emplace(FilesystemPath(image.file_path()), image);
  }
  return image_map;
}

proto::ImageInfo ImageLibrary::infoByFile(FilesystemPath filename) {
  for (auto image : library.images()) {
    if (image.file_path() == filename) {
      return image;
    }
  }
  return proto::ImageInfo();
}

std::string ImageLibrary::name(FilesystemPath filename) {
  return infoByFile(filename).name();
}

std::vector<std::string> ImageLibrary::tags(FilesystemPath filename) {
  proto::ImageInfo image = infoByFile(filename);
  std::vector<std::string> tags;
  for (auto tag : image.tags()) {
    insertIntoSortedVector(tags, tag);
  }
  return tags;
}

void ImageLibrary::addImage(FilesystemPath file, std::string name,
                            std::vector<std::string> tags) {
  proto::ImageInfo *new_image = library.add_images();
  new_image->set_file_path(file.string());
  new_image->set_name(name);
  for (auto tag : tags) {
    new_image->add_tags(tag);
  }
}

void ImageLibrary::clearLibrary() { library.Clear(); }

void ImageLibrary::saveLibrary() {
  Persistence::getInstance()->saveImageLibrary(library);
}

ImageSearchResults ImageLibrary::search(std::string query) {
  if (query == "") {
    // Empty search is a special case where we just return all
    return emptySearch();
  }

  auto tags = allTags();
  if (binary_search(tags.begin(), tags.end(), query)) {
    return exactMatchSearch(query);
  } else {
    return partialMatchSearch(query);
  }
}

ImageSearchResults ImageLibrary::emptySearch() {
  std::vector<proto::ImageInfo> matched_images;
  for (auto image : library.images()) {
    matched_images.push_back(image);
  }
  return ImageSearchResults(matched_images, "", allTags());
}

ImageSearchResults ImageLibrary::exactMatchSearch(std::string query) {
  std::vector<proto::ImageInfo> matched_images;
  for (auto image : library.images()) {
    if (std::find(image.tags().begin(), image.tags().end(), query) !=
        image.tags().end()) {
      matched_images.push_back(image);
    }
  }
  if (matched_images.size() > 0) {
    return ImageSearchResults(matched_images, query,
                              std::vector<std::string>({query}));
  }
  return ImageSearchResults(matched_images, query, std::vector<std::string>());
}

ImageSearchResults ImageLibrary::partialMatchSearch(std::string query) {
  std::vector<proto::ImageInfo> matched_images;
  std::vector<std::string> matched_tags;
  for (auto image : library.images()) {
    for (auto tag : image.tags()) {
      bool image_matched = false;
      if (std::search(tag.begin(), tag.end(), query.begin(), query.end()) !=
          tag.end()) {
        if (!image_matched) {
          matched_images.push_back(image);
        }
        image_matched = true;
        insertIntoSortedVector(matched_tags, tag);
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

std::vector<FilesystemPath> ImageSearchResults::filenames() {
  std::vector<FilesystemPath> files;
  for (auto image : matched_images) {
    files.push_back(FilesystemPath(image.file_path()));
  }
  return files;
}

std::vector<std::string> ImageSearchResults::matchedTags() {
  std::vector<std::string> tags;
  for (auto tag : matched_tag_list) {
    tags.push_back(tag);
  }
  return tags;
}

}  // namespace cszb_scoreboard