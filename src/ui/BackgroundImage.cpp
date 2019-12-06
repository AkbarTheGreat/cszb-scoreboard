
/*
ui/BackgroundImage.cpp: This class builds an image to serve as the background
for ScreenText

Copyright 2019 Tracy Beck

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

#include "ui/BackgroundImage.h"

namespace cszb_scoreboard {
BackgroundImage::BackgroundImage(wxSize size, Color color)
    : wxImage(size, true) {
  wxRect fullMask(0, 0, size.GetWidth(), size.GetHeight());
  SetRGB(fullMask, color.red(), color.green(), color.blue());
}

BackgroundImage BackgroundImage::errorImage(wxSize size) {
  BackgroundImage error(size, Color("White"));
  return error;
}

}  // namespace cszb_scoreboard
