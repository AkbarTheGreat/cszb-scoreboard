/*
ui/widget/Panel.cpp: A panel represents an interior panel container that is held
within other frames or panels.

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

#include "ui/widget/Panel.h"

namespace cszb_scoreboard {

Panel::~Panel() {
  if (should_self_delete) {
    wx->Destroy();
  }
}

auto Panel::button(const std::string &label) const -> std::unique_ptr<Button> {
  return std::make_unique<Button>(new swx::Button(wx, wxID_ANY, label));
}

auto Panel::label(const std::string &text) const -> std::unique_ptr<Label> {
  return std::make_unique<Label>(new swx::StaticText(wx, wxID_ANY, text));
}

auto Panel::panel() const -> std::unique_ptr<Panel> {
  return std::make_unique<Panel>(childPanel());
}

auto Panel::searchBox(const std::string &initial_text) const
    -> std::unique_ptr<SearchBox> {
  auto search_box = std::make_unique<SearchBox>(
      new swx::SearchCtrl(wx, wxID_ANY), initial_text);
  search_box->showSearchButton(false);
  search_box->showCancelButton(true);
  return search_box;
}

}  // namespace cszb_scoreboard
