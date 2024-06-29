#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ScoreboardCommon.h"  // for SCOREBOARD_VERSION
#include "config/ImageLibrary.h"
#include "image_library.pb.h"
#include "test/mocks/util/MockSingleton.h"
#include "ui/event/LibraryScanTimer.h"

#define TEST_STUB_PERSISTENT_TIMER
#define TEST_STUB_SINGLETON
#include "test/mocks/Stubs.h"

using ::testing::_;
using ::testing::Return;

namespace cszb_scoreboard::test {

// NOLINTBEGIN
class MockImageLibrary : public ImageLibrary {
 public:
  explicit MockImageLibrary(MockSingleton *singleton)
      : ImageLibrary(SingletonClass{}, singleton, proto::ImageLibrary()) {}
  MOCK_METHOD(void, detectLibraryChanges, (bool delete_missing), (override));
};
// NOLINTEND

class LibraryScanTimerTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockImageLibrary> image_library;

  LibraryScanTimerTest() = default;
  ~LibraryScanTimerTest() override = default;

  void SetUp() override {
    singleton = std::make_unique<MockSingleton>();
    image_library = std::make_unique<MockImageLibrary>(singleton.get());
    EXPECT_CALL(*singleton, imageLibrary())
        .WillRepeatedly(Return(image_library.get()));
  }

  void TearDown() override {
    singleton.reset();
    image_library.reset();
  }
};

TEST_F(LibraryScanTimerTest, DoesNothingAtStartup) {
  // Will never call detectLibraryChanges before cleanup (no immediate runs)
  EXPECT_CALL(*image_library, detectLibraryChanges(_)).Times(0);
  LibraryScanTimer timer(singleton.get());
}

TEST_F(LibraryScanTimerTest, AutomaticallyUpdatesOnTrigger) {
  // Will call with delete_missing=false on trigger
  EXPECT_CALL(*image_library, detectLibraryChanges(false)).Times(1);
  // Will never call with delete_missing=true
  EXPECT_CALL(*image_library, detectLibraryChanges(true)).Times(0);
  LibraryScanTimer timer(singleton.get());
  timer.trigger();
}

}  // namespace cszb_scoreboard::test
