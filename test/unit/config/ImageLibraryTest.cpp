/*
test/ImageLibraryTest.cpp: Tests for config/ImageLibrary

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

#include <algorithm>  // for max
#include <memory>     // for unique_ptr
#include <string>     // for operator+, allocator, cha...
#include <vector>     // for vector

#include "config/ImageLibrary.h"            // for ImageLibrary, ImageSearch...
#include "gmock/gmock.h"                    // for ElementsAre, MakePredicat...
#include "gtest/gtest.h"                    // for Test, Message, TestPartRe...
#include "image_library.pb.h"               // for ImageInfo, ImageLibrary
#include "test/mocks/util/MockSingleton.h"  // for MockSingleton
#include "util/FilesystemPath.h"            // for FilesystemPath
#include "util/Singleton.h"                 // for SingletonClass

namespace cszb_scoreboard::test {

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;

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
  EXPECT_THAT(tags, ElementsAre("cute", "dog", "gender", "neutral", "rodent",
                                "short", "Stall", "tall"));

  tags = tagStrings(library, true);
  // Check that image names are now included approprately.
  EXPECT_THAT(tags, ElementsAre("Bathroom", "capybara", "corgi", "cute", "dog",
                                "gender", "great dane", "neutral", "rodent",
                                "short", "Stall", "tall"));
}

// Ensures that the allFilenames method does not make paths absolute.
TEST(ImageLibraryTest, AllFilenamesBuildsCorrectly) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files,
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
}

// Searches for full single words match the relevant tag if it exists.
TEST(ImageLibraryTest, FullWordSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  // Simple search works
  auto result = library.search("dog");
  EXPECT_THAT(result.matchedTags(), ElementsAre("dog"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg"));

  // Potentially conflicting search doesn't collide
  result = library.search("tall");
  EXPECT_THAT(result.matchedTags(), ElementsAre("tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(LIB_ROOT + "great_dane.jpg")));

  // Titles match full names
  result = library.search("capybara");
  EXPECT_THAT(result.matchedTags(), ElementsAre("capybara"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(NONLIB_ROOT + "capy.jpg")));

  // Bad search returns nothing
  result = library.search("notgonnafindit");
  EXPECT_TRUE(result.matchedTags().empty());
  EXPECT_TRUE(result.filenames().empty());
}

// Searches for words that are not a tag return all partially matching tags.
TEST(ImageLibraryTest, PartialWordSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // Simple search works
  auto result = library.search("do");
  EXPECT_THAT(result.matchedTags(), ElementsAre("dog"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg"));

  // Potentially conflicting search doesn't collide
  result = library.search("tal");
  EXPECT_THAT(result.matchedTags(), ElementsAre("Stall", "tall"));
  EXPECT_THAT(result.filenames(), ElementsAre(LIB_ROOT + "great_dane.jpg",
                                              LIB_ROOT + "but-why.jpg"));

  // Titles match partial names too
  result = library.search("capy");
  EXPECT_THAT(result.matchedTags(), ElementsAre("capybara"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath({NONLIB_ROOT + "capy.jpg"})));

  // Empty search returns everything
  result = library.search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "rodent", "short",
                          "Stall", "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));

  // Bad search still returns nothing
  result = library.search("notgonnafindit");
  EXPECT_TRUE(result.matchedTags().empty());
  EXPECT_TRUE(result.filenames().empty());
}

// Makes sure that the de-duplication logic works correctly
TEST(ImageLibraryTest, DeduplicatingSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // This single-letter search should match 3/4 images
  auto result = library.search("c");
  EXPECT_THAT(result.matchedTags(), ElementsAre("capybara", "corgi", "cute"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg"));
}

// Makes sure that case insensitive searches work correctly
TEST(ImageLibraryTest, CaseInsensitiveSearches) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // This should match exactly one image
  auto result = library.search("stall");
  EXPECT_THAT(result.matchedTags(), ElementsAre("Stall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(LIB_ROOT + "but-why.jpg")));
  // This should match exactly one image
  result = library.search("bath");
  EXPECT_THAT(result.matchedTags(), ElementsAre("Bathroom"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(LIB_ROOT + "but-why.jpg")));
}

TEST(ImageLibraryTest, RemoveRoot) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.removeLibraryRoot();
  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files,
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  ASSERT_EQ(library.libraryRoot(), "");
}

TEST(ImageLibraryTest, MoveRoot) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.moveLibraryRoot(FilesystemPath(NONLIB_ROOT));
  // The one relative path in the library (great_dane) moves implicitly, all
  // other paths remain the same.
  auto result = library.search("");
  EXPECT_THAT(
      result.filenames(),
      ElementsAre(LIB_ROOT + "corgi.jpg", NONLIB_ROOT + "great_dane.jpg",
                  NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files,
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  ASSERT_EQ(library.libraryRoot(), NONLIB_ROOT);
}

TEST(ImageLibraryTest, SetRoot) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.setLibraryRoot(FilesystemPath(NONLIB_ROOT));
  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files,
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                          "capy.jpg", LIB_ROOT + "but-why.jpg"));
  ASSERT_EQ(library.libraryRoot(), NONLIB_ROOT);
}

TEST(ImageLibraryTest, AddImage) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  library.addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});
  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files, ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                                 NONLIB_ROOT + "capy.jpg",
                                 LIB_ROOT + "but-why.jpg", "new-image.png"));
  auto result = library.search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "rodent", "short",
                          "Stall", "tag_test", "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg",
                          LIB_ROOT + "new-image.png"));
}

TEST(ImageLibraryTest, MoveImage) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // No change as it's not an exact match.
  library.moveImage(FilesystemPath("capy.jpg"),
                    FilesystemPath("moved-image.png"));
  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files,
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  // Changes for exact match.
  library.moveImage(FilesystemPath(NONLIB_ROOT + "capy.jpg"),
                    FilesystemPath("moved-image.png"));
  files = library.allFilenames();
  EXPECT_THAT(files, ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                                 "moved-image.png", LIB_ROOT + "but-why.jpg"));
  // Search result also updates.
  auto result = library.search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "rodent", "short",
                          "Stall", "tall"));
  EXPECT_THAT(
      result.filenames(),
      ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                  LIB_ROOT + "moved-image.png", LIB_ROOT + "but-why.jpg"));
}

TEST(ImageLibraryTest, DeleteImage) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  // No deletion as it's not an exact match.
  library.deleteImage(FilesystemPath("capy.jpg"));
  std::vector<FilesystemPath> files = library.allFilenames();
  EXPECT_THAT(files,
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  // Deletes an exact match.
  library.deleteImage(FilesystemPath(NONLIB_ROOT + "capy.jpg"));
  files = library.allFilenames();
  EXPECT_THAT(files, ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                                 LIB_ROOT + "but-why.jpg"));
  // Search result and tags also update.
  auto result = library.search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "short", "Stall",
                          "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", LIB_ROOT + "great_dane.jpg",
                          LIB_ROOT + "but-why.jpg"));
}

TEST(ImageLibraryTest, TemporaryClone) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  auto temp = library.temporaryClone();
  ASSERT_NE(&library, temp.get());

  temp->addImage(FilesystemPath("temp/tempImage.jpg"), "Temp Only",
                 {"tempTag"});

  EXPECT_THAT(library.allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  EXPECT_THAT(temp->allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg",
                          "temp/tempImage.jpg"));
}

TEST(ImageLibraryTest, ClearLibrary) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  EXPECT_FALSE(library.allFilenames().empty());

  library.clearLibrary();

  EXPECT_TRUE(library.allFilenames().empty());
}

TEST(ImageLibraryTest, CopyFrom) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);
  ImageLibrary copy_target = testLibrary(&singleton);

  copy_target.clearLibrary();

  // Verify starting state.
  EXPECT_THAT(library.allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  EXPECT_TRUE(copy_target.allFilenames().empty());

  {
    auto temp_copy = library.temporaryClone();
    copy_target.copyFrom(*temp_copy);
  }

  // Copy succeeded
  EXPECT_THAT(library.allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  EXPECT_THAT(copy_target.allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));

  copy_target.addImage(FilesystemPath("newImage.jpg"), "new Image", {"newtag"});

  // Ensure that the copies are still separate.
  EXPECT_THAT(library.allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg"));
  EXPECT_THAT(copy_target.allFilenames(),
              ElementsAre(LIB_ROOT + "corgi.jpg", "great_dane.jpg",
                          NONLIB_ROOT + "capy.jpg", LIB_ROOT + "but-why.jpg",
                          "newImage.jpg"));
}

TEST(ImageLibraryTest, GetName) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  EXPECT_EQ("great dane", library.name(FilesystemPath("great_dane.jpg")));

  EXPECT_EQ("", library.name(FilesystemPath("notgonnafindit.jpg")));
}

TEST(ImageLibraryTest, GetTags) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  EXPECT_THAT(library.tags(FilesystemPath(LIB_ROOT + "corgi.jpg")),
              ElementsAre(CaseOptionalString("cute"), CaseOptionalString("dog"),
                          CaseOptionalString("short")));

  EXPECT_TRUE(library.tags(FilesystemPath("notgonnafindit.jpg")).empty());
}

TEST(ImageLibraryTest, SetName) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  library.setName(FilesystemPath("great_dane.jpg"), "Amazing dane");
  EXPECT_EQ("Amazing dane", library.name(FilesystemPath("great_dane.jpg")));

  library.setName(FilesystemPath("invalid.jpg"), "Not a file");
  EXPECT_EQ("", library.name(FilesystemPath("invalid.jpg")));
}

TEST(ImageLibraryTest, SetTags) {
  MockSingleton singleton;
  ImageLibrary library = testLibrary(&singleton);

  library.setTags(FilesystemPath(LIB_ROOT + "corgi.jpg"),
                  {"floof", "dog", "long"});
  // Contains exactly the list set, not a merge or a union of the previous set,
  // and returns it in alphabetical order.
  EXPECT_THAT(
      library.tags(FilesystemPath(LIB_ROOT + "corgi.jpg")),
      ElementsAre(CaseOptionalString("dog"), CaseOptionalString("floof"),
                  CaseOptionalString("long")));

  library.setTags(FilesystemPath("invalid.jpg"), {"notafile"});
  EXPECT_TRUE(library.tags(FilesystemPath("invalid.jpg")).empty());
}

// TODO (akbar): Test smartUpdateLibraryRoot -- this will likely either require
// sophisticated mocking of FilesystemPath or a test filestructure that's
// created/destroyed at setup/teardown.

}  // namespace cszb_scoreboard::test
