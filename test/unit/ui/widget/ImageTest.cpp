/*
test/unit/ui/widget/ImageTest.cpp: Tests for ui/widget/Image

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

#include <gtest/gtest.h>
#include <wx/image.h>
#include <wx/log.h>

#include <string>
#include <vector>

#include "ui/graphics/Color.h"
#include "ui/widget/Image.h"
#include "util/Base64.h"

// IWYU pragma: no_include "gmock/gmock.h"
// IWYU pragma: no_include "gtest/gtest.h"
// IWYU pragma: no_include <gtest/gtest_pred_impl.h>
// IWYU pragma: no_include "gtest/gtest_pred_impl.h"

namespace cszb_scoreboard::test {

// Hex sequence for a minimal static 1x1 transparent GIF
const std::string STATIC_GIF_BASE64 =
    "R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7";

// Valid Base64 sequence for a 2-frame 1x1 animated GIF
const std::string ANIMATED_GIF_BASE64 =
    "R0lGODlhAQABAIAAAAAAAP///yH/"
    "C05FVFNDQVBFMi4wAwEAAAAh+"
    "QQACgAAACwAAAAAAQABAAACAkQBACH5BAAKAAAALAAAAAABAAEAAAICTAEAOw==";

class ImageTest : public ::testing::Test {
 protected:
  void SetUp() override { wxInitAllImageHandlers(); }
};

TEST_F(ImageTest, StaticImageLoading) {
  wxLogNull logNo;
  std::vector<char> data;
  Base64::decode(STATIC_GIF_BASE64, &data);
  Image image(data);

  EXPECT_TRUE(image.isOk());
  EXPECT_FALSE(image.isAnimated());
  EXPECT_EQ(image.size().width, 1);
  EXPECT_EQ(image.size().height, 1);
}

TEST_F(ImageTest, AnimatedImageLoadingAndPlayback) {
  wxLogNull logNo;
  std::vector<char> data;
  Base64::decode(ANIMATED_GIF_BASE64, &data);
  Image image(data);

  EXPECT_TRUE(image.isOk());
  EXPECT_TRUE(image.isAnimated());
  EXPECT_EQ(image.size().width, 1);
  EXPECT_EQ(image.size().height, 1);

  // Default frame animation verification
  wxImage frame0 = image.animate(0);
  EXPECT_TRUE(Image(frame0).isOk());

  // Rescaling also rescales the frames
  image.rescale(4, 4);
  EXPECT_EQ(image.size().width, 4);
  EXPECT_EQ(image.size().height, 4);

  wxImage scaled_frame = image.animate(0);
  EXPECT_TRUE(Image(scaled_frame).isOk());
  EXPECT_EQ(scaled_frame.GetWidth(), 4);
  EXPECT_EQ(scaled_frame.GetHeight(), 4);
}

TEST_F(ImageTest, SetColorWorks) {
  wxLogNull logNo;
  Image image(Size{.width = 10, .height = 10});
  EXPECT_TRUE(image.isOk());
  EXPECT_EQ(image.size().width, 10);
  EXPECT_EQ(image.size().height, 10);

  Color color("Red");
  image.color(color);
  EXPECT_EQ(image.red(0, 0), color.Red());
  EXPECT_EQ(image.green(0, 0), color.Green());
  EXPECT_EQ(image.blue(0, 0), color.Blue());
}

}  // namespace cszb_scoreboard::test
