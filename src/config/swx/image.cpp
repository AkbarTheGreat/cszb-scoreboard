/*
config/swx/image.cpp: A wrapper around wxImage with methods removing the need to
use wx types directly.

Copyright 2021 Tracy Beck

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

#include "config/swx/image.h"

namespace cszb_scoreboard {

Image::Image(const wxImage &img) : wxImage() {
  this->m_refData = CloneRefData(img.GetRefData());
}

auto Image::size() -> ::cszb_scoreboard::Size {
  return ::cszb_scoreboard::Size::fromWx(GetSize());
}

}  // namespace cszb_scoreboard
