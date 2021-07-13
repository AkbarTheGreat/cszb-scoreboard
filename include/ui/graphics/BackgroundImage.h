/*
ui/BackgroundImage.h: This class builds an image to serve as the background
for ScreenText

Copyright 2019-2021 Tracy Beck

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

#include <wx/image.h>  // for wxImage

class wxSize;

namespace cszb_scoreboard {
class Color;

class BackgroundImage : public wxImage {
 public:
  BackgroundImage(wxSize size, Color color);
  static auto errorImage(wxSize size) -> BackgroundImage;
};

}  // namespace cszb_scoreboard
