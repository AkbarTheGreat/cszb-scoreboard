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
#include <filesystem>
#include <memory>  // for unique_ptr
#include <string>  // for operator+, allocator, cha...
#include <vector>  // for vector

#include "config/ImageLibrary.h"  // for ImageLibrary, ImageSearch...
#include "gmock/gmock.h"          // for ElementsAre, MakePredicat...
#include "gtest/gtest.h"          // for Test, Message, TestPartRe...
#include "image_library.pb.h"     // for ImageInfo, ImageLibrary
#include "test/mocks/config/MockPersistence.h"
#include "test/mocks/util/MockSingleton.h"  // for MockSingleton
#include "test/util/TempFilesystem.h"
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/Singleton.h"       // for SingletonClass

#define TEST_STUB_SINGLETON
#include "test/mocks/Stubs.h"

namespace cszb_scoreboard::test {

using ::testing::ElementsAre;
using ::testing::Return;
using ::testing::UnorderedElementsAre;

const std::string LIB_ROOT_DIR = "test";
const std::string NONLIB_ROOT_DIR = "test2";

class ImageLibraryTest : public ::testing::Test {
 protected:
  std::unique_ptr<ImageLibrary> library;
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockPersistence> persist;
  std::unique_ptr<TempFilesystem> filesystem;

  ImageLibraryTest() = default;
  ~ImageLibraryTest() override = default;

  void SetUp() override {
    singleton = std::make_unique<MockSingleton>();
    library = testLibrary();
    persist = std::make_unique<MockPersistence>(singleton.get());
    EXPECT_CALL(*singleton, persistence).WillRepeatedly(Return(persist.get()));
  }

  void TearDown() override {
    library.reset();
    singleton.reset();
    persist->resetGeneralConfig();
    persist.reset();
    filesystem.reset();
  }

  auto internalPath(const std::string &base, const std::string &subpath) {
    auto path = std::filesystem::temp_directory_path();
    if (filesystem != nullptr) {
      path = filesystem->getRoot();
    }
    path /= base;
    if (!subpath.empty()) {
      path /= subpath;
    }
    return path.string();
  }

  auto libRoot(const std::string &subpath) {
    return internalPath(LIB_ROOT_DIR, subpath);
  }

  auto libRoot() { return libRoot(""); }

  auto nonlibRoot(const std::string &subpath) {
    return internalPath(NONLIB_ROOT_DIR, subpath);
  }

  auto nonlibRoot() { return nonlibRoot(""); }

  auto testLibrary() -> std::unique_ptr<ImageLibrary> {
    proto::ImageLibrary library;

    library.set_library_root(libRoot());

    // Add a corgi
    proto::ImageInfo *image = library.add_images();
    image->set_name("corgi");
    image->set_file_path(libRoot("corgi.jpg"));
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
    image->set_file_path(nonlibRoot("capy.jpg"));
    image->add_tags("rodent");
    image->add_tags("cute");
    image->add_tags("short");

    // Add a bathroom
    image = library.add_images();
    image->set_name("Bathroom");
    image->set_file_path(libRoot(
        "but-why.jpg"));  // Because we can use (s)tall for partial matches
    image->add_tags("gender");
    image->add_tags("neutral");
    image->add_tags("Stall");

    return std::make_unique<ImageLibrary>(SingletonClass{}, singleton.get(),
                                          library);
  }

  void addImageToSubdir(const std::string &subdir, const std::string &name) {
    std::filesystem::path path = subdir;
    path /= name;
    filesystem->createFile(path.string(), ".");
  }

  // Creates a filesystem to match the entries in testLibrary.  This is not done
  // for every test to save IO time.
  void buildFilesystem() {
    filesystem = std::make_unique<TempFilesystem>();
    filesystem->createSubdir(LIB_ROOT_DIR);
    filesystem->createSubdir(NONLIB_ROOT_DIR);
    addImageToSubdir(LIB_ROOT_DIR, "corgi.jpg");
    addImageToSubdir(LIB_ROOT_DIR, "great_dane.jpg");
    addImageToSubdir(NONLIB_ROOT_DIR, "capy.jpg");
    addImageToSubdir(LIB_ROOT_DIR, "but-why.jpg");
    // Rebuild the library to match the filesystem.
    library = testLibrary();
  }
};

auto tagStrings(const ImageLibrary &library,
                bool include_name = false) -> std::vector<std::string> {
  std::vector<CaseOptionalString> tags = library.allTags(include_name);
  std::vector<std::string> tag_strings;
  tag_strings.reserve(tags.size());
  for (const auto &tag : tags) {
    tag_strings.emplace_back(tag.string());
  }
  return tag_strings;
}

TEST_F(ImageLibraryTest, AllTagsBuildsCorrectly) {
  std::vector<std::string> tags = tagStrings(*library);
  // Check that they're in the correct (alphabetical) order.
  EXPECT_THAT(tags, ElementsAre("cute", "dog", "gender", "neutral", "rodent",
                                "short", "Stall", "tall"));

  tags = tagStrings(*library, true);
  // Check that image names are now included approprately.
  EXPECT_THAT(tags, ElementsAre("Bathroom", "capybara", "corgi", "cute", "dog",
                                "gender", "great dane", "neutral", "rodent",
                                "short", "Stall", "tall"));
}

// Ensures that the allFilenames method does not make paths absolute.
TEST_F(ImageLibraryTest, AllFilenamesBuildsCorrectly) {
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
}

// Searches for full single words match the relevant tag if it exists.
TEST_F(ImageLibraryTest, FullWordSearches) {
  // Simple search works
  auto result = library->search("dog");
  EXPECT_THAT(result.matchedTags(), ElementsAre("dog"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg")));

  // Potentially conflicting search doesn't collide
  result = library->search("tall");
  EXPECT_THAT(result.matchedTags(), ElementsAre("tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(libRoot("great_dane.jpg"))));

  // Titles match full names
  result = library->search("capybara");
  EXPECT_THAT(result.matchedTags(), ElementsAre("capybara"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(nonlibRoot("capy.jpg"))));

  // Bad search returns nothing
  result = library->search("notgonnafindit");
  EXPECT_TRUE(result.matchedTags().empty());
  EXPECT_TRUE(result.filenames().empty());
}

// Searches for words that are not a tag return all partially matching tags.
TEST_F(ImageLibraryTest, PartialWordSearches) {
  // Simple search works
  auto result = library->search("do");
  EXPECT_THAT(result.matchedTags(), ElementsAre("dog"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg")));

  // Potentially conflicting search doesn't collide
  result = library->search("tal");
  EXPECT_THAT(result.matchedTags(), ElementsAre("Stall", "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("great_dane.jpg"), libRoot("but-why.jpg")));

  // Titles match partial names too
  result = library->search("capy");
  EXPECT_THAT(result.matchedTags(), ElementsAre("capybara"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath({nonlibRoot("capy.jpg")})));

  // Empty search returns everything
  result = library->search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "rodent", "short",
                          "Stall", "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));

  // Bad search still returns nothing
  result = library->search("notgonnafindit");
  EXPECT_TRUE(result.matchedTags().empty());
  EXPECT_TRUE(result.filenames().empty());
}

// Makes sure that the de-duplication logic works correctly
TEST_F(ImageLibraryTest, DeduplicatingSearches) {
  // This single-letter search should match 3/4 images
  auto result = library->search("c");
  EXPECT_THAT(result.matchedTags(), ElementsAre("capybara", "corgi", "cute"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          nonlibRoot("capy.jpg")));
}

// Makes sure that case insensitive searches work correctly
TEST_F(ImageLibraryTest, CaseInsensitiveSearches) {
  // This should match exactly one image
  auto result = library->search("stall");
  EXPECT_THAT(result.matchedTags(), ElementsAre("Stall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(libRoot("but-why.jpg"))));
  // This should match exactly one image
  result = library->search("bath");
  EXPECT_THAT(result.matchedTags(), ElementsAre("Bathroom"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(FilesystemPath(libRoot("but-why.jpg"))));
}

TEST_F(ImageLibraryTest, RemoveRoot) {
  library->removeLibraryRoot();
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  ASSERT_EQ(library->libraryRoot(), "");
}

TEST_F(ImageLibraryTest, MoveRoot) {
  library->moveLibraryRoot(FilesystemPath(nonlibRoot()));
  // The one relative path in the library (great_dane) moves implicitly, all
  // other paths remain the same.
  auto result = library->search("");
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), nonlibRoot("great_dane.jpg"),
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  ASSERT_EQ(library->libraryRoot(), nonlibRoot());
}

TEST_F(ImageLibraryTest, SetRoot) {
  library->setLibraryRoot(FilesystemPath(nonlibRoot()));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          "capy.jpg", libRoot("but-why.jpg")));
  ASSERT_EQ(library->libraryRoot(), nonlibRoot());
}

TEST_F(ImageLibraryTest, AddImage) {
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                                 "new-image.png"));
  auto result = library->search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "rodent", "short",
                          "Stall", "tag_test", "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                          libRoot("new-image.png")));
}

TEST_F(ImageLibraryTest, MoveImage) {
  // No change as it's not an exact match.
  library->moveImage(FilesystemPath("capy.jpg"),
                     FilesystemPath("moved-image.png"));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  // Changes for exact match.
  library->moveImage(FilesystemPath(nonlibRoot("capy.jpg")),
                     FilesystemPath("moved-image.png"));
  files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 "moved-image.png", libRoot("but-why.jpg")));
  // Search result also updates.
  auto result = library->search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "rodent", "short",
                          "Stall", "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          libRoot("moved-image.png"), libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, DeleteImage) {
  // No deletion as it's not an exact match.
  library->deleteImage(FilesystemPath("capy.jpg"));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  // Deletes an exact match.
  library->deleteImage(FilesystemPath(nonlibRoot("capy.jpg")));
  files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 libRoot("but-why.jpg")));
  // Search result and tags also update.
  auto result = library->search("");
  EXPECT_THAT(result.matchedTags(),
              ElementsAre("cute", "dog", "gender", "neutral", "short", "Stall",
                          "tall"));
  EXPECT_THAT(result.filenames(),
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, TemporaryClone) {
  auto temp = library->temporaryClone();
  ASSERT_NE(library.get(), temp.get());

  temp->addImage(FilesystemPath("temp/tempImage.jpg"), "Temp Only",
                 {"tempTag"});

  EXPECT_THAT(library->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  EXPECT_THAT(temp->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                          "temp/tempImage.jpg"));
}

TEST_F(ImageLibraryTest, ClearLibrary) {
  EXPECT_FALSE(library->allFilenames().empty());

  library->clearLibrary();

  EXPECT_TRUE(library->allFilenames().empty());
}

TEST_F(ImageLibraryTest, CopyFrom) {
  auto copy_target = testLibrary();

  copy_target->clearLibrary();

  // Verify starting state.
  EXPECT_THAT(library->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  EXPECT_TRUE(copy_target->allFilenames().empty());

  {
    auto temp_copy = library->temporaryClone();
    copy_target->copyFrom(*temp_copy);
  }

  // Copy succeeded
  EXPECT_THAT(library->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  EXPECT_THAT(copy_target->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));

  copy_target->addImage(FilesystemPath("newImage.jpg"), "new Image",
                        {"newtag"});

  // Ensure that the copies are still separate.
  EXPECT_THAT(library->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
  EXPECT_THAT(copy_target->allFilenames(),
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                          "newImage.jpg"));
}

TEST_F(ImageLibraryTest, GetName) {
  EXPECT_EQ("great dane", library->name(FilesystemPath("great_dane.jpg")));

  EXPECT_EQ("", library->name(FilesystemPath("notgonnafindit.jpg")));
}

TEST_F(ImageLibraryTest, GetTags) {
  EXPECT_THAT(library->tags(FilesystemPath(libRoot("corgi.jpg"))),
              ElementsAre(CaseOptionalString("cute"), CaseOptionalString("dog"),
                          CaseOptionalString("short")));

  EXPECT_TRUE(library->tags(FilesystemPath("notgonnafindit.jpg")).empty());
}

TEST_F(ImageLibraryTest, SetName) {
  library->setName(FilesystemPath("great_dane.jpg"), "Amazing dane");
  EXPECT_EQ("Amazing dane", library->name(FilesystemPath("great_dane.jpg")));

  library->setName(FilesystemPath("invalid.jpg"), "Not a file");
  EXPECT_EQ("", library->name(FilesystemPath("invalid.jpg")));
}

TEST_F(ImageLibraryTest, SetTags) {
  library->setTags(FilesystemPath(libRoot("corgi.jpg")),
                   {"floof", "dog", "long"});
  // Contains exactly the list set, not a merge or a union of the previous set,
  // and returns it in alphabetical order.
  EXPECT_THAT(
      library->tags(FilesystemPath(libRoot("corgi.jpg"))),
      ElementsAre(CaseOptionalString("dog"), CaseOptionalString("floof"),
                  CaseOptionalString("long")));

  library->setTags(FilesystemPath("invalid.jpg"), {"notafile"});
  EXPECT_TRUE(library->tags(FilesystemPath("invalid.jpg")).empty());
}

TEST_F(ImageLibraryTest, SmartUpdateNoDuplicates) {
  buildFilesystem();
  library->smartUpdateLibraryRoot(FilesystemPath(nonlibRoot()));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          "capy.jpg", libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, SmartUpdateDuplicateRelativePaths) {
  buildFilesystem();
  addImageToSubdir(NONLIB_ROOT_DIR, "great_dane.jpg");
  library->smartUpdateLibraryRoot(FilesystemPath(nonlibRoot()));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 "capy.jpg", libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, SmartUpdateDuplicateAbsolutePaths) {
  buildFilesystem();
  // Corgi will still be referred to from the original lib path.
  addImageToSubdir(NONLIB_ROOT_DIR, "corgi.jpg");
  library->smartUpdateLibraryRoot(FilesystemPath(nonlibRoot()));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"),
                          "capy.jpg", libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, SmartUpdateMissingAbsoluteFile) {
  buildFilesystem();
  library->addImage(FilesystemPath(libRoot("new-image.png")), "New Thing",
                    {"tag_test"});
  library->smartUpdateLibraryRoot(FilesystemPath(nonlibRoot()));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(
      files,
      ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"), "capy.jpg",
                  libRoot("but-why.jpg"), libRoot("new-image.png")));
}

TEST_F(ImageLibraryTest, SmartUpdateMissingRelativeFile) {
  buildFilesystem();
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});
  library->smartUpdateLibraryRoot(FilesystemPath(nonlibRoot()));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(
      files,
      ElementsAre(libRoot("corgi.jpg"), libRoot("great_dane.jpg"), "capy.jpg",
                  libRoot("but-why.jpg"), libRoot("new-image.png")));
}

TEST_F(ImageLibraryTest, LoadSaveLibrary) {
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});

  library->saveLibrary();

  ImageLibrary loaded(SingletonClass{}, singleton.get(),
                      persist->loadImageLibrary());
  std::vector<std::string> tags = tagStrings(*library);
  // Check that they're in the correct (alphabetical) order.
  EXPECT_THAT(tags, ElementsAre("cute", "dog", "gender", "neutral", "rodent",
                                "short", "Stall", "tag_test", "tall"));
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                                 "new-image.png"));
}

TEST_F(ImageLibraryTest, DetectChangesNoChanges) {
  buildFilesystem();
  LibraryUpdateResults results =
      library->detectLibraryChanges(/* delete_missing = */ false);
  EXPECT_TRUE(results.addedImages().empty());
  EXPECT_TRUE(results.movedImages().empty());
  EXPECT_TRUE(results.removedImages().empty());
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, DetectChangesAddsFile) {
  buildFilesystem();
  addImageToSubdir(libRoot(), "new-image.png");
  LibraryUpdateResults results =
      library->detectLibraryChanges(/* delete_missing = */ false);
  EXPECT_TRUE(results.movedImages().empty());
  EXPECT_TRUE(results.removedImages().empty());
  EXPECT_EQ(results.addedImages().size(), 1);
  EXPECT_EQ(results.addedImages()[0].added().file_path(), "new-image.png");
  EXPECT_EQ(results.addedImages()[0].added().name(), "New Image");
  EXPECT_EQ(results.addedImages()[0].removed().file_path(), "");
  EXPECT_EQ(results.addedImages()[0].removed().name(), "");
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                                 "new-image.png"));
  EXPECT_EQ(library->name(FilesystemPath("new-image.png")), "New Image");
  EXPECT_TRUE(library->tags(FilesystemPath("new-image.png")).empty());
}

TEST_F(ImageLibraryTest, DetectChangesIgnoresNonImages) {
  buildFilesystem();
  addImageToSubdir(libRoot(), "new-image.txt");
  LibraryUpdateResults results =
      library->detectLibraryChanges(/*delete_missing = */ false);
  EXPECT_TRUE(results.addedImages().empty());
  EXPECT_TRUE(results.movedImages().empty());
  EXPECT_TRUE(results.removedImages().empty());
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
}

TEST_F(ImageLibraryTest, DetectChangesLeavesMissingFile) {
  buildFilesystem();
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});
  LibraryUpdateResults results =
      library->detectLibraryChanges(/*delete_missing = */ false);
  EXPECT_TRUE(results.addedImages().empty());
  EXPECT_TRUE(results.movedImages().empty());
  EXPECT_TRUE(results.removedImages().empty());
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                                 "new-image.png"));
}

TEST_F(ImageLibraryTest, DetectChangesMovesFile) {
  buildFilesystem();
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});

  filesystem->createSubdir(libRoot("subdir"));
  addImageToSubdir(libRoot("subdir"), "new-image.png");
  std::string expected_path =
      (std::filesystem::path("subdir") / "new-image.png").string();

  LibraryUpdateResults results =
      library->detectLibraryChanges(/*delete_missing = */ false);
  EXPECT_TRUE(results.addedImages().empty());
  EXPECT_TRUE(results.removedImages().empty());
  EXPECT_EQ(results.movedImages().size(), 1);
  EXPECT_EQ(results.movedImages()[0].added().file_path(), expected_path);
  EXPECT_EQ(results.movedImages()[0].added().name(), "New Thing");
  EXPECT_EQ(results.movedImages()[0].removed().file_path(), "new-image.png");
  EXPECT_EQ(results.movedImages()[0].removed().name(), "New Thing");
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                                 expected_path));
  EXPECT_EQ(library->name(FilesystemPath(expected_path)), "New Thing");
  EXPECT_THAT(library->tags(FilesystemPath(expected_path)),
              ElementsAre(CaseOptionalString("tag_test")));
}

TEST_F(ImageLibraryTest, DetectChangesMovesFileWithDeleteEnabled) {
  buildFilesystem();
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});

  filesystem->createSubdir(libRoot("subdir"));
  addImageToSubdir(libRoot("subdir"), "new-image.png");
  std::string expected_path =
      (std::filesystem::path("subdir") / "new-image.png").string();

  LibraryUpdateResults results =
      library->detectLibraryChanges(/*delete_missing = */ true);
  EXPECT_TRUE(results.addedImages().empty());
  EXPECT_TRUE(results.removedImages().empty());
  EXPECT_EQ(results.movedImages().size(), 1);
  EXPECT_EQ(results.movedImages()[0].added().file_path(), expected_path);
  EXPECT_EQ(results.movedImages()[0].added().name(), "New Thing");
  EXPECT_EQ(results.movedImages()[0].removed().file_path(), "new-image.png");
  EXPECT_EQ(results.movedImages()[0].removed().name(), "New Thing");
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files, ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                                 nonlibRoot("capy.jpg"), libRoot("but-why.jpg"),
                                 expected_path));
  EXPECT_EQ(library->name(FilesystemPath(expected_path)), "New Thing");
  EXPECT_THAT(library->tags(FilesystemPath(expected_path)),
              ElementsAre(CaseOptionalString("tag_test")));
}

TEST_F(ImageLibraryTest, DetectChangesCanRemoveMissingFile) {
  buildFilesystem();
  library->addImage(FilesystemPath("new-image.png"), "New Thing", {"tag_test"});
  LibraryUpdateResults results =
      library->detectLibraryChanges(/*delete_missing = */ true);
  EXPECT_TRUE(results.addedImages().empty());
  EXPECT_TRUE(results.movedImages().empty());
  EXPECT_EQ(results.removedImages().size(), 1);
  EXPECT_EQ(results.removedImages()[0].added().file_path(), "");
  EXPECT_EQ(results.removedImages()[0].added().name(), "");
  EXPECT_EQ(results.removedImages()[0].removed().file_path(), "new-image.png");
  EXPECT_EQ(results.removedImages()[0].removed().name(), "New Thing");
  std::vector<FilesystemPath> files = library->allFilenames();
  EXPECT_THAT(files,
              ElementsAre(libRoot("corgi.jpg"), "great_dane.jpg",
                          nonlibRoot("capy.jpg"), libRoot("but-why.jpg")));
}

}  // namespace cszb_scoreboard::test
