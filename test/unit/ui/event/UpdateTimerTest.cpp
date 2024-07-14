#include <gtest/gtest.h>

#include "ScoreboardCommon.h"  // for SCOREBOARD_VERSION
#include "test/mocks/ui/frame/MockMainView.h"
#include "test/mocks/ui/widget/swx/MockFrame.h"
#include "test/mocks/util/MockHttpReader.h"
#include "test/mocks/util/MockSingleton.h"
#include "ui/event/UpdateTimer.h"
#include "util/AutoUpdate.h"

#define TEST_STUB_MAIN_VIEW
#define TEST_STUB_PERSISTENT_TIMER
#include "test/mocks/Stubs.h"

using ::testing::_;
using ::testing::Return;

namespace cszb_scoreboard::test {

// NOLINTBEGIN
class MockAutoUpdate : public AutoUpdate {
 public:
  explicit MockAutoUpdate(MockSingleton *singleton)
      : AutoUpdate(SingletonClass{}, singleton,
                   std::make_unique<MockHttpReader>()) {}
  MOCK_METHOD(bool, checkForUpdate, (const std::string &current_version),
              (override));
  MOCK_METHOD(bool, updateIsDownloadable, (), (const, override));
  MOCK_METHOD(void, removeOldUpdate, (), (override));
  MOCK_METHOD(bool, updateInPlace, (), (override));
  MOCK_METHOD(bool, checkForUpdate,
              (const std::string &current_version,
               const std::string &platform_name),
              (override));
};
// NOLINTEND

class UpdateTimerTest : public ::testing::Test {
 protected:
  std::unique_ptr<UpdateTimer> timer;
  std::unique_ptr<MockSingleton> singleton;
  std::unique_ptr<MockMainView> main_view;
  std::unique_ptr<swx::MockFrame> ui_frame;
  std::unique_ptr<MockAutoUpdate> auto_update;

  UpdateTimerTest() = default;
  ~UpdateTimerTest() override = default;

  void SetUp() override {
    singleton = std::make_unique<MockSingleton>();
    ui_frame = std::make_unique<swx::MockFrame>();
    main_view = std::make_unique<MockMainView>(ui_frame.get(), singleton.get());
    auto_update = std::make_unique<MockAutoUpdate>(singleton.get());
    EXPECT_CALL(*singleton, autoUpdate())
        .WillRepeatedly(Return(auto_update.get()));
  }

  void TearDown() override {
    timer.reset();
    singleton.reset();
    main_view.reset();
    ui_frame.reset();
    auto_update.reset();
  }
};

TEST_F(UpdateTimerTest, ConstructorCallsRemove) {
  EXPECT_CALL(*auto_update, removeOldUpdate()).Times(1);
  UpdateTimer update_timer(main_view.get(), singleton.get());
}

TEST_F(UpdateTimerTest, NoVersionAvailable) {
  // Simulate running the newest version
  EXPECT_CALL(*auto_update, checkForUpdate(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(*ui_frame, SetStatusText(_, _)).Times(0);
  UpdateTimer update_timer(main_view.get(), singleton.get());
}

TEST_F(UpdateTimerTest, NewVersionAvailableNoDownload) {
  // Simulate a new version being available
  EXPECT_CALL(*auto_update, checkForUpdate(_)).WillRepeatedly(Return(true));
  // Report that it cannot be downloaded automatically
  EXPECT_CALL(*auto_update, updateIsDownloadable())
      .WillRepeatedly(Return(false));
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(
          wxString("New version found, please go to "
                   "github.com/AkbarTheGreat/cszb-scoreboard to update."),
          _))
      .Times(1);
  UpdateTimer update_timer(main_view.get(), singleton.get());
}

TEST_F(UpdateTimerTest, NewVersionAvailableAutoupdateSuceeds) {
  // Simulate a new version being available
  EXPECT_CALL(*auto_update, checkForUpdate(_)).WillOnce(Return(true));
  EXPECT_CALL(*auto_update, updateIsDownloadable()).WillOnce(Return(true));
  EXPECT_CALL(*auto_update, updateInPlace()).WillOnce(Return(true));
  EXPECT_CALL(*ui_frame,
              SetStatusText(wxString("New version found, downloading..."), _))
      .Times(1);
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(
          wxString("Auto-update downloaded.  Please restart to apply."), _))
      .Times(1);
  UpdateTimer update_timer(main_view.get(), singleton.get());
}

TEST_F(UpdateTimerTest, NewVersionAvailableAutoupdateFails) {
  // Simulate a new version being available
  EXPECT_CALL(*auto_update, checkForUpdate(_)).WillOnce(Return(true));
  EXPECT_CALL(*auto_update, updateIsDownloadable()).WillOnce(Return(true));
  EXPECT_CALL(*auto_update, updateInPlace()).WillOnce(Return(false));
  EXPECT_CALL(*ui_frame,
              SetStatusText(wxString("New version found, downloading..."), _))
      .Times(1);
  EXPECT_CALL(
      *ui_frame,
      SetStatusText(
          wxString("Auto-update failed!  Please manually update scoreboard."),
          _))
      .Times(1);
  UpdateTimer update_timer(main_view.get(), singleton.get());
}

}  // namespace cszb_scoreboard::test
