/*
test/unit/config/PersistenceTest.cpp: Tests for config/Persistence

Copyright 2026 Tracy Beck

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

#include <gtest/gtest.h>  // for TestInfo (ptr only), EXPE...
#include <wx/gdicmn.h>    // for wxTheColourDatabase, wxCo...

#include <memory>  // for allocator, unique_ptr

#include "config.pb.h"                      // for TeamConfig, DisplayConfig
#include "config/Persistence.h"             // for Persistence
#include "gtest/gtest.h"                    // for Message, TestPartResult
#include "image_library.pb.h"               // for ImageLibrary
#include "slide_show.pb.h"                  // for SlideShow
#include "team_library.pb.h"                // for TeamLibrary, TeamLibInfo
#include "test/mocks/util/MockSingleton.h"  // for MockSingleton
#include "ui/graphics/Color.h"              // for Color
#include "util/ProtoUtil.h"                 // for ProtoUtil
#include "util/Singleton.h"                 // for SingletonClass

namespace cszb_scoreboard::test {

class PersistenceTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockSingleton> singleton;

  void SetUp() override {
    if (wxTheColourDatabase == nullptr) {
      wxTheColourDatabase = new wxColourDatabase();
    }
    singleton = std::make_unique<MockSingleton>();
  }

  void TearDown() override { singleton.reset(); }
};

TEST_F(PersistenceTest, DisplaysConfig) {
  Persistence persistence(SingletonClass{}, singleton.get());

  proto::DisplayConfig config;
  auto* display = config.add_displays();
  display->set_id(12);

  persistence.saveDisplays(config);
  proto::DisplayConfig loaded = persistence.loadDisplays();
  EXPECT_EQ(loaded.displays_size(), 1);
  EXPECT_EQ(loaded.displays(0).id(), 12);
}

TEST_F(PersistenceTest, GeneralConfig) {
  Persistence persistence(SingletonClass{}, singleton.get());

  proto::GeneralConfig config;
  config.set_drop_shadow_depth(4.5);

  persistence.saveGeneralConfig(config);
  proto::GeneralConfig loaded = persistence.loadGeneralConfig();
  EXPECT_EQ(loaded.drop_shadow_depth(), 4.5);
}

TEST_F(PersistenceTest, SlideShow) {
  Persistence persistence(SingletonClass{}, singleton.get());

  proto::SlideShow slideshow;
  slideshow.set_delay(12.5);

  persistence.saveSlideShow(slideshow);
  proto::SlideShow loaded = persistence.loadSlideShow();
  EXPECT_EQ(loaded.delay(), 12.5);
}

TEST_F(PersistenceTest, Teams) {
  Persistence persistence(SingletonClass{}, singleton.get());

  proto::TeamConfig config;
  auto* team = config.add_teams();
  team->set_team_type(proto::TeamInfo_TeamType_HOME_TEAM);
  ProtoUtil::protoClr(Color("Blue"), team->mutable_team_color());

  persistence.saveTeams(config);
  proto::TeamConfig loaded = persistence.loadTeams();
  EXPECT_EQ(loaded.teams_size(), 1);
  EXPECT_EQ(loaded.teams(0).team_type(), proto::TeamInfo_TeamType_HOME_TEAM);
  EXPECT_EQ(ProtoUtil::wxClr(loaded.teams(0).team_color()), Color("Blue"));
}

TEST_F(PersistenceTest, ImageLibrary) {
  Persistence persistence(SingletonClass{}, singleton.get());

  proto::ImageLibrary library;
  library.set_library_root("my_images");

  persistence.saveImageLibrary(library);
  proto::ImageLibrary loaded = persistence.loadImageLibrary();
  EXPECT_EQ(loaded.library_root(), "my_images");
}

TEST_F(PersistenceTest, TeamLibrary) {
  Persistence persistence(SingletonClass{}, singleton.get());

  // Default team library has placeholder Boston teams loaded during fake init
  proto::TeamLibrary library = persistence.loadTeamLibrary();
  EXPECT_GT(library.teams_size(), 0);

  proto::TeamLibrary new_library;
  auto* t = new_library.add_teams();
  t->set_name("New Team");

  persistence.saveTeamLibrary(new_library);
  proto::TeamLibrary loaded = persistence.loadTeamLibrary();
  EXPECT_EQ(loaded.teams_size(), 1);
  EXPECT_EQ(loaded.teams(0).name(), "New Team");
}

}  // namespace cszb_scoreboard::test
