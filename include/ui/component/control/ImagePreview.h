/*
ui/component/control/ImagePreview.h: A preview thumbnail of an image,
selectable to send to a ScreenText.

Copyright 2020-2023 Tracy Beck

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

#include <optional>  // for optional

#include "config/swx/image.h"     // for Image
#include "ui/widget/Panel.h"      // for Panel
#include "util/FilesystemPath.h"  // for FilesystemPath

namespace cszb_scoreboard {
struct Size;
class RenderContext;

namespace swx {
class Panel;
}  // namespace swx

class ImagePreview : public Panel {
 public:
  explicit ImagePreview(swx::Panel *wx);

  void clearImage();
  [[nodiscard]] auto getFilename() const -> std::optional<FilesystemPath>;
  void setImage(const FilesystemPath &filename);

 private:
  void bindEvents();
  void paintEvent(RenderContext *renderer);
  static auto ratio(const Size &size) -> float;

  std::optional<FilesystemPath> filename;
  Image image;
};

}  // namespace cszb_scoreboard
