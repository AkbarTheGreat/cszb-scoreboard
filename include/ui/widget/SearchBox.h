/*
ui/widget/SearchBox.h: A search box control.

Copyright 2021-2024 Tracy Beck

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

#include "ui/widget/Widget.h"
#include "ui/widget/swx/SearchCtrl.h"

namespace cszb_scoreboard {

class SearchBox : public Widget {
 public:
  SearchBox(swx::SearchCtrl *search, const std::string &initial_text) {
    _wx = search;
    _wx->SetDescriptiveText(initial_text);
  }

  void showSearchButton(bool show) { _wx->ShowSearchButton(show); }
  void showCancelButton(bool show) { _wx->ShowCancelButton(show); }
  auto value() -> std::string { return _wx->GetValue().ToStdString(); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::SearchCtrl *_wx;
};

}  // namespace cszb_scoreboard
