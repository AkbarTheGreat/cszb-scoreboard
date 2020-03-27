/*
test/ImageLibraryTest.cpp: Tests for config/ImageLibrary

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
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace cszb_scoreboard {
namespace test {

ImageLibrary testLibrary() {
  proto::ImageLibrary library;

  // Add a corgi
  proto::ImageInfo *image = library.add_images();
  image->set_name("corgi");
  image->set_file_path("/test/corgi.jpg");
  image->add_tags("dog");
  image->add_tags("cute");
  image->add_tags("short");

  // Add a great dane
  image = library.add_images();
  image->set_name("great dane");
  image->set_file_path("/test/great_dane.jpg");
  image->add_tags("dog");
  image->add_tags("cute");
  image->add_tags("tall");

  // Add a capybara
  image = library.add_images();
  image->set_name("capybara");
  image->set_file_path("/test/capy.jpg");
  image->add_tags("rodent");
  image->add_tags("cute");
  image->add_tags("short");

  // Add a bathroom
  image = library.add_images();
  image->set_name("bathroom");
  image->set_file_path(
      "/test/but-why.jpg");  // Because we can us stall for partial matches
  image->add_tags("gender");
  image->add_tags("neutral");
  image->add_tags("stall");

  return ImageLibrary(library);
}

TEST(ImageLibraryTest, AllTagsBuildsCorrectly) {
  ImageLibrary library = testLibrary();
  std::vector<std::string> tags = library.allTags();
  // Our test data has 8 unique tags.
  EXPECT_EQ(tags.size(), 8);
  // Check that they're in the correct (alphabetical) order.
  ASSERT_THAT(tags, ::testing::ElementsAre("cute", "dog", "gender", "neutral",
                                           "rodent", "short", "stall", "tall"));
}

// Searches for full single words match the relevant tag if it exists.
TEST(ImageLibraryTest, FullWordSearches) {
  ImageLibrary library = testLibrary();
  // Simple search works
  auto result = library.search("dog");
  ASSERT_THAT(result.matched_tags(), ::testing::ElementsAre("dog"));
  ASSERT_THAT(
      result.filenames(),
      ::testing::ElementsAre("/test/corgi.jpg", "/test/great_dane.jpg"));

  // Potentially conflicting search doesn't collide
  result = library.search("tall");
  ASSERT_THAT(result.matched_tags(), ::testing::ElementsAre("tall"));
  ASSERT_THAT(result.filenames(),
              ::testing::ElementsAre("/test/great_dane.jpg"));

  // Bad search returns nothing
  result = library.search("notgonnafindit");
  ASSERT_THAT(result.matched_tags(), ::testing::ElementsAre());
  ASSERT_THAT(result.filenames(), ::testing::ElementsAre());
}

// Searches for words that are not a tag return all partially matching tags.
TEST(ImageLibraryTest, PartialWordSearches) {
  ImageLibrary library = testLibrary();
  // Simple search works
  auto result = library.search("do");
  ASSERT_THAT(result.matched_tags(), ::testing::ElementsAre("dog"));
  ASSERT_THAT(
      result.filenames(),
      ::testing::ElementsAre("/test/corgi.jpg", "/test/great_dane.jpg"));

  // Potentially conflicting search doesn't collide
  result = library.search("tal");
  ASSERT_THAT(result.matched_tags(), ::testing::ElementsAre("stall", "tall"));
  ASSERT_THAT(result.filenames(), ::testing::ElementsAre("/test/great_dane.jpg",
                                                         "/test/but-why.jpg"));

  // Empty search returns everything
  result = library.search("");
  ASSERT_THAT(result.matched_tags(),
              ::testing::ElementsAre("cute", "dog", "gender", "neutral",
                                     "rodent", "short", "stall", "tall"));
  ASSERT_THAT(result.filenames(),
              ::testing::ElementsAre("/test/corgi.jpg", "/test/great_dane.jpg",
                                     "/test/capy.jpg", "/test/but-why.jpg"));

  // Bad search still returns nothing
  result = library.search("notgonnafindit");
  ASSERT_THAT(result.matched_tags(), ::testing::ElementsAre());
  ASSERT_THAT(result.filenames(), ::testing::ElementsAre());
}

}  // namespace test
}  // namespace cszb_scoreboard
