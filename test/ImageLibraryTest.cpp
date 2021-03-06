/*
test/ImageLibraryTest.cpp: Tests for config/ImageLibrary

Copyright 2020-2021 Tracy Beck

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
#include "gtest/gtest.h"

namespace cszb_scoreboard::test {

auto testLibrary() -> ImageLibrary {
  proto::ImageLibrary library;

  // Add a corgi
  proto::ImageInfo* image = library.add_images();
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
      "/test/but-why.jpg");  // Because we can use (s)tall for partial matches
  image->add_tags("gender");
  image->add_tags("neutral");
  image->add_tags("stall");

  return ImageLibrary(library);
}

// A couple of convenience defines to handle vector comparisons
#define ASSERT_VECTOR(a, b) ASSERT_PRED_FORMAT2(assertVectorEquality, a, b)
#define ASSERT_PATH_VECTOR(a, ...) \
  ASSERT_VECTOR(a, filesystemPathVector(__VA_ARGS__))
#define ASSERT_STR_VECTOR(a, ...) \
  ASSERT_VECTOR(a, std::vector<std::string>(__VA_ARGS__))

// Predicate for asserting against two arbitrary vectors.
template <typename T, typename A>
auto assertVectorEquality(const char* actual_expression,
                          const char* expected_expression,
                          std::vector<T, A> const& actual,
                          std::vector<T, A> const& expected)
    -> ::testing::AssertionResult {
  if (actual.size() != expected.size()) {
    return ::testing::AssertionFailure()
           << actual_expression << " and " << expected_expression
           << " have differing sizes (" << actual.size() << ", "
           << expected.size() << ")";
  }

  for (int i = 0; i < actual.size(); i++) {
    if (actual[i] != expected[i]) {
      return ::testing::AssertionFailure()
             << actual_expression << " and " << expected_expression
             << " differ beginning at index " << i << " (" << actual[i] << ", "
             << expected[i] << ")";
    }
  }

  return ::testing::AssertionSuccess();
}

// Quick and easy cast of a list of strings to a list of paths
auto filesystemPathVector(const std::vector<std::string>& in)
    -> std::vector<FilesystemPath> {
  std::vector<FilesystemPath> out;
  for (const auto& i : in) {
    //  These vectors are small enough that the extra code isn't worthwhile, so
    //  waive this lint error.
    // NOLINTNEXTLINE(performance-inefficient-vector-operation)
    out.emplace_back(FilesystemPath(i));
  }
  return out;
}

// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST(ImageLibraryTest, AllTagsBuildsCorrectly) {
  ImageLibrary library = testLibrary();
  std::vector<std::string> tags = library.allTags();
  // Check that they're in the correct (alphabetical) order.
  ASSERT_STR_VECTOR(tags, {"cute", "dog", "gender", "neutral", "rodent",
                           "short", "stall", "tall"});
}

// Searches for full single words match the relevant tag if it exists.
// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST(ImageLibraryTest, FullWordSearches) {
  ImageLibrary library = testLibrary();

  // Simple search works
  auto result = library.search("dog");
  ASSERT_STR_VECTOR(result.matchedTags(), {"dog"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {"/test/corgi.jpg", "/test/great_dane.jpg"});

  // Potentially conflicting search doesn't collide
  result = library.search("tall");
  ASSERT_STR_VECTOR(result.matchedTags(), {"tall"});
  ASSERT_PATH_VECTOR(result.filenames(), {"/test/great_dane.jpg"});

  // Bad search returns nothing
  result = library.search("notgonnafindit");
  ASSERT_STR_VECTOR(result.matchedTags(), {});
  ASSERT_PATH_VECTOR(result.filenames(), {});
}

// Searches for words that are not a tag return all partially matching tags.
// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST(ImageLibraryTest, PartialWordSearches) {
  ImageLibrary library = testLibrary();
  // Simple search works
  auto result = library.search("do");
  ASSERT_STR_VECTOR(result.matchedTags(), {"dog"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {"/test/corgi.jpg", "/test/great_dane.jpg"});

  // Potentially conflicting search doesn't collide
  result = library.search("tal");
  ASSERT_STR_VECTOR(result.matchedTags(), {"stall", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {"/test/great_dane.jpg", "/test/but-why.jpg"});

  // Empty search returns everything
  result = library.search("");
  ASSERT_STR_VECTOR(result.matchedTags(), {"cute", "dog", "gender", "neutral",
                                           "rodent", "short", "stall", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {"/test/corgi.jpg", "/test/great_dane.jpg",
                      "/test/capy.jpg", "/test/but-why.jpg"});

  // Bad search still returns nothing
  result = library.search("notgonnafindit");
  ASSERT_STR_VECTOR(result.matchedTags(), {});
  ASSERT_PATH_VECTOR(result.filenames(), {});
}

// Makes sure that the de-duplication logic works correctly
// NOLINTNEXTLINE until https://reviews.llvm.org/D90835 is released.
TEST(ImageLibraryTest, DeduplicatingSearches) {
  ImageLibrary library = testLibrary();
  // This single-letter search should match 3/4 images
  auto result = library.search("o");
  ASSERT_STR_VECTOR(result.matchedTags(), {"dog", "rodent", "short"});
  ASSERT_PATH_VECTOR(
      result.filenames(),
      {"/test/corgi.jpg", "/test/great_dane.jpg", "/test/capy.jpg"});
}

}  // namespace cszb_scoreboard::test
