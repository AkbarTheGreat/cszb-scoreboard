/*
test/ImageLibraryTest.cpp: Tests for config/ImageLibrary

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

#include <gtest/gtest-message.h>    // for Message
#include <gtest/gtest-test-part.h>  // for TestPartResult
#include <gtest/gtest.h>            // IWYU pragma: keep

#include <filesystem>  // for operator<<, operator==
#include <sstream>     // for basic_stringbuf<>::int_type
#include <string>      // for string, operator+, allocator
#include <vector>      // for vector

#include "config/ImageLibrary.h"            // for ImageLibrary, ImageSearch...
#include "image_library.pb.h"               // for ImageInfo, ImageLibrary
#include "test/mocks/util/MockSingleton.h"  // for MockSingleton
#include "util/FilesystemPath.h"            // for FilesystemPath
#include "util/Singleton.h"                 // for SingletonClass
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard::test {

#ifdef _WIN32
const std::string LIB_ROOT = "C:\\test\\";
const std::string NONLIB_ROOT = "C:\\test2\\";
#else   // ifdef _WIN32
const std::string LIB_ROOT = "/test/";
const std::string NONLIB_ROOT = "/test2/";
#endif  // ifdef _WIN32

auto testLibrary(MockSingleton *singleton) -> ImageLibrary {
  proto::ImageLibrary library;

  library.set_library_root(LIB_ROOT);

  // Add a corgi
  proto::ImageInfo *image = library.add_images();
  image->set_name("corgi");
  image->set_file_path(LIB_ROOT + "corgi.jpg");
  image->add_tags("dog");
  image->add_tags("cute");
  image->add_tags("short");

  // Add a great dane
  image = library.add_images();
  image->set_name("great dane");
  image->set_file_path("great_dane.jpg");
  image->add_tags("dog");
  image->add_tags("cute");
  image->add_tags("tall");
  image->set_is_relative(true);

  // Add a capybara
  image = library.add_images();
  image->set_name("capybara");
  image->set_file_path(NONLIB_ROOT + "capy.jpg");
  image->add_tags("rodent");
  image->add_tags("cute");
  image->add_tags("short");

  // Add a bathroom
  image = library.add_images();
  image->set_name("Bathroom");
  image->set_file_path(
      LIB_ROOT +
      "but-why.jpg");  // Because we can use (s)tall for partial matches
  image->add_tags("gender");
  image->add_tags("neutral");
  image->add_tags("Stall");

  return ImageLibrary(SingletonClass{}, singleton, library);
}

// A couple of convenience defines to handle vector comparisons
#define ASSERT_VECTOR(a, b) ASSERT_PRED_FORMAT2(assertVectorEquality, a, b)
#define ASSERT_PATH_VECTOR(a, ...) \
  ASSERT_VECTOR(a, filesystemPathVector(__VA_ARGS__))
#define ASSERT_STR_VECTOR(a, ...) \
  ASSERT_VECTOR(a, std::vector<std::string>(__VA_ARGS__))

// Predicate for asserting against two arbitrary vectors.
template <typename T, typename A>
auto assertVectorEquality(const char *actual_expression,
                          const char *expected_expression,
                          std::vector<T, A> const &actual,
                          std::vector<T, A> const &expected)
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
auto filesystemPathVector(const std::vector<std::string> &in)
    -> std::vector<FilesystemPath> {
  std::vector<FilesystemPath> out;
  out.reserve(in.size());
  for (const auto &i : in) {
    out.emplace_back(FilesystemPath(i));
  }
  return out;
}

auto tagStrings(const ImageLibrary &library, bool include_name = false)
    -> std::vector<std::string> {
  std::vector<CaseOptionalString> tags = library.allTags(include_name);
  std::vector<std::string> tag_strings;
  tag_strings.reserve(tags.size());
  for (const auto &tag : tags) {
    tag_strings.emplace_back(tag.string());
  }
  return tag_strings;
}

TEST(ImageLibraryTest, AllTagsBuildsCorrectly) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  std::vector<std::string> tags = tagStrings(library);
  // Check that they're in the correct (alphabetical) order.
  ASSERT_STR_VECTOR(tags, {"cute", "dog", "gender", "neutral", "rodent",
                           "short", "Stall", "tall"});

  tags = tagStrings(library, true);
  // Check that image names are now included approprately.
  ASSERT_STR_VECTOR(
      tags, {"Bathroom", "capybara", "corgi", "cute", "dog", "gender",
             "great dane", "neutral", "rodent", "short", "Stall", "tall"});
}

// Ensures that the allFilenames method does not make paths absolute.
TEST(ImageLibraryTest, AllFilenamesBuildsCorrectly) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(files,
                     {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});
}

// Searches for full single words match the relevant tag if it exists.
TEST(ImageLibraryTest, FullWordSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  // Simple search works
  auto result = library.search("dog");
  ASSERT_STR_VECTOR(result.matchedTags(), {"dog"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg"});

  // Potentially conflicting search doesn't collide
  result = library.search("tall");
  ASSERT_STR_VECTOR(result.matchedTags(), {"tall"});
  ASSERT_PATH_VECTOR(result.filenames(), {LIB_ROOT + "great_dane.jpg"});

  // Titles match full names
  result = library.search("capybara");
  ASSERT_STR_VECTOR(result.matchedTags(), {"capybara"});
  ASSERT_PATH_VECTOR(result.filenames(), {NONLIB_ROOT + "capy.jpg"});

  // Bad search returns nothing
  result = library.search("notgonnafindit");
  ASSERT_STR_VECTOR(result.matchedTags(), {});
  ASSERT_PATH_VECTOR(result.filenames(), {});
}

// Searches for words that are not a tag return all partially matching tags.
TEST(ImageLibraryTest, PartialWordSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // Simple search works
  auto result = library.search("do");
  ASSERT_STR_VECTOR(result.matchedTags(), {"dog"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg"});

  // Potentially conflicting search doesn't collide
  result = library.search("tal");
  ASSERT_STR_VECTOR(result.matchedTags(), {"Stall", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "great_dane.jpg", LIB_ROOT + "but-why.jpg"});

  // Titles match partial names too
  result = library.search("capy");
  ASSERT_STR_VECTOR(result.matchedTags(), {"capybara"});
  ASSERT_PATH_VECTOR(result.filenames(), {NONLIB_ROOT + "capy.jpg"});

  // Empty search returns everything
  result = library.search("");
  ASSERT_STR_VECTOR(result.matchedTags(), {"cute", "dog", "gender", "neutral",
                                           "rodent", "short", "Stall", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});

  // Bad search still returns nothing
  result = library.search("notgonnafindit");
  ASSERT_STR_VECTOR(result.matchedTags(), {});
  ASSERT_PATH_VECTOR(result.filenames(), {});
}

// Makes sure that the de-duplication logic works correctly
TEST(ImageLibraryTest, DeduplicatingSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // This single-letter search should match 3/4 images
  auto result = library.search("c");
  ASSERT_STR_VECTOR(result.matchedTags(), {"capybara", "corgi", "cute"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg"});
}

// Makes sure that case insensitive searches work correctly
TEST(ImageLibraryTest, CaseInsensitiveSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // This should match exactly one image
  auto result = library.search("stall");
  ASSERT_STR_VECTOR(result.matchedTags(), {"Stall"});
  ASSERT_PATH_VECTOR(result.filenames(), {LIB_ROOT + "but-why.jpg"});
  // This should match exactly one image
  result = library.search("bath");
  ASSERT_STR_VECTOR(result.matchedTags(), {"Bathroom"});
  ASSERT_PATH_VECTOR(result.filenames(), {LIB_ROOT + "but-why.jpg"});
}

TEST(ImageLibraryTest, RemoveRoot) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.removeLibraryRoot();
  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(files,
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});
  ASSERT_EQ(library.libraryRoot(), "");
}

TEST(ImageLibraryTest, MoveRoot) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.moveLibraryRoot(FilesystemPath(NONLIB_ROOT));
  // The one relative path in the library (great_dane) moves implicitly, all
  // other paths remain the same.
  auto result = library.search("");
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", NONLIB_ROOT + "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});
  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(files,
                     {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});
  ASSERT_EQ(library.libraryRoot(), NONLIB_ROOT);
}

TEST(ImageLibraryTest, SetRoot) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.setLibraryRoot(FilesystemPath(NONLIB_ROOT));
  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(
      files, {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg", "capy.jpg",
              LIB_ROOT + "but-why.jpg"});
  ASSERT_EQ(library.libraryRoot(), NONLIB_ROOT);
}

TEST(ImageLibraryTest, AddImage) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});
  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(files, {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                             NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg",
                             "new-image.png"});
  auto result = library.search("");
  ASSERT_STR_VECTOR(result.matchedTags(),
                    {"cute", "dog", "gender", "neutral", "rodent", "short",
                     "Stall", "tag_test", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg",
                      LIB_ROOT + "new-image.png"});
}

TEST(ImageLibraryTest, MoveImage) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // No change as it's not an exact match.
  library.moveImage(FilesystemPath("capy.jpg"),
                    FilesystemPath("moved-image.png"));
  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(files,
                     {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});
  // Changes for exact match.
  library.moveImage(FilesystemPath(NONLIB_ROOT + "capy.jpg"),
                    FilesystemPath("moved-image.png"));
  files = library.allFilenames();
  ASSERT_PATH_VECTOR(files, {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                             "moved-image.png", LIB_ROOT + "but-why.jpg"});
  // Search result also updates.
  auto result = library.search("");
  ASSERT_STR_VECTOR(result.matchedTags(), {"cute", "dog", "gender", "neutral",
                                           "rodent", "short", "Stall", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                      LIB_ROOT + "moved-image.png", LIB_ROOT + "but-why.jpg"});
}

TEST(ImageLibraryTest, DeleteImage) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // No deletion as it's not an exact match.
  library.deleteImage(FilesystemPath("capy.jpg"));
  std::vector<FilesystemPath> files = library.allFilenames();
  ASSERT_PATH_VECTOR(files,
                     {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                      NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"});
  // Deletes an exact match.
  library.deleteImage(FilesystemPath(NONLIB_ROOT + "capy.jpg"));
  files = library.allFilenames();
  ASSERT_PATH_VECTOR(files, {LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                             LIB_ROOT + "but-why.jpg"});
  // Search result and tags also update.
  auto result = library.search("");
  ASSERT_STR_VECTOR(result.matchedTags(), {"cute", "dog", "gender", "neutral",
                                           "short", "Stall", "tall"});
  ASSERT_PATH_VECTOR(result.filenames(),
                     {LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                      LIB_ROOT + "but-why.jpg"});
}

}  // namespace cszb_scoreboard::test
