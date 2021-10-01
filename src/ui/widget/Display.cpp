/*
ui/widget/Display.cpp: A representation of a physical display.  This is notable
within the widgets classes in that it owns it's underlying wx object and
construct it itself in normal cases, and has an injectible constructor for
testing.

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

#include "ui/widget/Display.h"

#include "util/ProtoUtil.h"

namespace cszb_scoreboard {

Display ::Display(unsigned int index)
    : Display(std::make_shared<swx::Display>(index)) {}

void Display::geometry(proto::Rectangle* rectangle) {
  ProtoUtil::protoRct(wx->GetGeometry(), rectangle);
}

}  // namespace cszb_scoreboard
