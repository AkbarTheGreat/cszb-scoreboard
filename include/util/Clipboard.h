/*
util/Clipboard.h: Static methods to handle clipboard manipulation.

Copyright 2021-2022 Tracy Beck

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

#pragma once

#include <optional>

#include "config/swx/clipbrd.h"
#include "config/swx/image.h"

namespace cszb_scoreboard {

class Clipboard {
 public:
  static auto getImage() -> std::optional<Image> {
    std::optional<Image> clip;
    if (wxTheClipboard->Open()) {
      if (wxTheClipboard->IsSupported(wxDF_BITMAP)) {
        wxBitmapDataObject data;
        wxTheClipboard->GetData(data);
        clip = Image(data.GetBitmap());
      }
      wxTheClipboard->Close();
    }
    return clip;
  }
};

}  // namespace cszb_scoreboard
