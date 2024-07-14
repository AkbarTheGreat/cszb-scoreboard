#include <gmock/gmock.h>  // for GMOCK_PP_INTERNAL_IF_0
#include <gtest/gtest.h>  // for TestInfo (ptr only)
#include <wx/string.h>    // for wxString, operator==

#include <algorithm>  // for max
#include <memory>     // for unique_ptr, allocator
#include <vector>     // for vector

#include "config/ImageLibrary.h"                 // for LibraryUpdateResults
#include "image_library.pb.h"                    // for ImageInfo, ImageLibrary
#include "test/mocks/ui/frame/MockMainView.h"    // for MockMainView
#include "test/mocks/ui/widget/swx/MockFrame.h"  // for MockFrame
#include "test/mocks/util/MockSingleton.h"       // for MockSingleton
#include "ui/event/LibraryScanTimer.h"           // for LibraryScanTimer
#include "util/Singleton.h"                      // for SingletonClass

#define TEST_STUB_MAIN_VIEW
#define TEST_STUB_PERSISTENT_TIMER
#define TEST_STUB_SINGLETON
#include "test/mocks/Stubs.h"  // for PersistentTimer::tri...

using ::testing::_;
using ::testing::Return;

namespace cszb_scoreboard::test {

// NOLINTBEGIN
class MockImageLibrary : public ImageLibrary {
 public:
  explicit MockImageLibrary(MockSingleton *singleton)
      : ImageLibrary(SingletonClass{}, singleton, proto::ImageLibrary()) {}
  MOCK_METHOD(LibraryUpdateResults, detectLibraryChanges, (bool delete_missing),
              (override));
};
// NOLINTEND

class LibraryScanTimerTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockImageLibrary> image_library;
  std::unique_ptr<MockMainView> main_view;
  std::unique_ptr<swx::MockFrame> ui_frame;

  LibraryScanTimerTest() = default;
  ~LibraryScanTimerTest() override = default;

  void SetUp() override {
    singleton = std::make_unique<MockSingleton>();
    ui_frame = std::make_unique<swx::MockFrame>();
    main_view = std::make_unique<MockMainView>(ui_frame.get(), singleton.get());
    image_library = std::make_unique<MockImageLibrary>(singleton.get());
    EXPECT_CALL(*singleton, imageLibrary())
        .WillRepeatedly(Return(image_library.get()));
  }

  void TearDown() override {
    singleton.reset();
    image_library.reset();
    main_view.reset();
    ui_frame.reset();
  }

  static auto foundChanges(int num) -> std::vector<ImageChange> {
    std::vector<ImageChange> changes;
    for (int i = 0; i < num; ++i) {
      // NOLINTNEXTLINE(performance-inefficient-vector-operation)
      changes.emplace_back(proto::ImageInfo(), proto::ImageInfo());
    }
    return changes;
  }

  static auto noChanges() -> std::vector<ImageChange> {
    return foundChanges(0);
  }
};

TEST_F(LibraryScanTimerTest, DoesNothingAtStartup) {
  // Will never call detectLibraryChanges before cleanup (no immediate runs)
  EXPECT_CALL(*image_library, detectLibraryChanges(_)).Times(0);
  LibraryScanTimer timer(main_view.get(), singleton.get());
}

TEST_F(LibraryScanTimerTest, SearchesOnTriggerFindsNothing) {
  // Will call with delete_missing=false on trigger
  EXPECT_CALL(*image_library, detectLibraryChanges(false))
      .WillOnce(
          Return(LibraryUpdateResults(noChanges(), noChanges(), noChanges())));
  // Will never call with delete_missing=true
  EXPECT_CALL(*image_library, detectLibraryChanges(true)).Times(0);
  EXPECT_CALL(*ui_frame, SetStatusText(_, _)).Times(0);
  LibraryScanTimer timer(main_view.get(), singleton.get());
  timer.trigger();
}

TEST_F(LibraryScanTimerTest, SearchesOnTriggerFindsAdditions) {
  // Will call with delete_missing=false on trigger
  EXPECT_CALL(*image_library, detectLibraryChanges(false))
      .WillOnce(Return(
          LibraryUpdateResults(foundChanges(2), noChanges(), noChanges())));
  // Will never call with delete_missing=true
  EXPECT_CALL(*image_library, detectLibraryChanges(true)).Times(0);
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(wxString("Library changes detected! Added 2 images."), _))
      .Times(1);
  LibraryScanTimer timer(main_view.get(), singleton.get());
  timer.trigger();
}

TEST_F(LibraryScanTimerTest, SearchesOnTriggerFindsMoves) {
  // Will call with delete_missing=false on trigger
  EXPECT_CALL(*image_library, detectLibraryChanges(false))
      .WillOnce(Return(
          LibraryUpdateResults(noChanges(), foundChanges(3), noChanges())));
  // Will never call with delete_missing=true
  EXPECT_CALL(*image_library, detectLibraryChanges(true)).Times(0);
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(wxString("Library changes detected! Moved 3 images."), _))
      .Times(1);
  LibraryScanTimer timer(main_view.get(), singleton.get());
  timer.trigger();
}

/* This scenario is currently impossible in production. */
TEST_F(LibraryScanTimerTest, SearchesOnTriggerFindsDeletes) {
  // Will call with delete_missing=false on trigger
  EXPECT_CALL(*image_library, detectLibraryChanges(false))
      .WillOnce(Return(
          LibraryUpdateResults(noChanges(), noChanges(), foundChanges(1))));
  // Will never call with delete_missing=true
  EXPECT_CALL(*image_library, detectLibraryChanges(true)).Times(0);
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(wxString("Library changes detected! Removed 1 images."), _))
      .Times(1);
  LibraryScanTimer timer(main_view.get(), singleton.get());
  timer.trigger();
}

TEST_F(LibraryScanTimerTest, SearchesOnTriggerFindsAdditionsAndMoves) {
  // Will call with delete_missing=false on trigger
  EXPECT_CALL(*image_library, detectLibraryChanges(false))
      .WillOnce(Return(
          LibraryUpdateResults(foundChanges(2), foundChanges(3), noChanges())));
  // Will never call with delete_missing=true
  EXPECT_CALL(*image_library, detectLibraryChanges(true)).Times(0);
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(
          wxString("Library changes detected! Added 2 images. Moved 3 images."),
          _))
      .Times(1);
  LibraryScanTimer timer(main_view.get(), singleton.get());
  timer.trigger();
}

}  // namespace cszb_scoreboard::test
