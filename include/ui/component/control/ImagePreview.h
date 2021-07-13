/*
ui/component/control/ImagePreview.h: A preview thumbnail of an image,
selectable to send to a ScreenText.

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

#include <wx/image.h>             // for wxImage
#include <optional>               // for optional

#include "ui/widget/Panel.h"      // for Panel
#include "util/FilesystemPath.h"  // for FilesystemPath

class wxDC;
class wxSize;

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

class ImagePreview : public Panel {
 public:
  explicit ImagePreview(swx::Panel* wx);

  void clearImage();
  [[nodiscard]] auto getFilename() const -> std::optional<FilesystemPath>;
  void setImage(const FilesystemPath& filename);

 private:
  void bindEvents();
  void paintEvent();
  static auto ratio(const wxSize& size) -> float;
  void renderImage(wxDC* dc);

  std::optional<FilesystemPath> filename;
  wxImage image;
};

}  // namespace cszb_scoreboard
