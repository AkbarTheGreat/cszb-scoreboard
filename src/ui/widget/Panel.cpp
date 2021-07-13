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

#include <wx/anybutton.h>
#include <wx/textctrl.h>

#include "ui/widget/Button.h"
#include "ui/widget/CheckBox.h"
#include "ui/widget/ColorPicker.h"
#include "ui/widget/Divider.h"
#include "ui/widget/Label.h"
#include "ui/widget/RadioButton.h"
#include "ui/widget/ScrollingPanel.h"
#include "ui/widget/SearchBox.h"
#include "ui/widget/Text.h"
#include "ui/widget/Toggle.h"
#include "ui/widget/swx/Button.h"
#include "ui/widget/swx/CheckBox.h"
#include "ui/widget/swx/ColourPickerCtrl.h"
#include "ui/widget/swx/RadioButton.h"
#include "ui/widget/swx/ScrolledWindow.h"
#include "ui/widget/swx/SearchCtrl.h"
#include "ui/widget/swx/StaticLine.h"
#include "ui/widget/swx/StaticText.h"
#include "ui/widget/swx/TextCtrl.h"
#include "ui/widget/swx/ToggleButton.h"

namespace cszb_scoreboard {

Panel::~Panel() {
  if (should_self_delete) {
    _wx()->Destroy();
  }
}

auto Panel::button(const std::string &label, bool exact_fit) const
    -> std::unique_ptr<Button> {
  if (exact_fit) {
    return std::make_unique<Button>(
        new swx::Button(_wx(), wxID_ANY, label, wxDefaultPosition,
                        wxDefaultSize, wxBU_EXACTFIT));
  }
  return std::make_unique<Button>(new swx::Button(_wx(), wxID_ANY, label));
}

auto Panel::checkBox(const std::string &label) const
    -> std::unique_ptr<CheckBox> {
  return std::make_unique<CheckBox>(new swx::CheckBox(_wx(), wxID_ANY, label));
}

auto Panel::colorPicker(const wxColour &initial_color) const
    -> std::unique_ptr<ColorPicker> {
  return std::make_unique<ColorPicker>(
      new swx::ColourPickerCtrl(_wx(), wxID_ANY, initial_color));
}

auto Panel::divider() const -> std::unique_ptr<Divider> {
  return std::make_unique<Divider>(new swx::StaticLine(_wx()));
}

auto Panel::label(const std::string &text) const -> std::unique_ptr<Label> {
  return std::make_unique<Label>(new swx::StaticText(_wx(), wxID_ANY, text));
}

auto Panel::panel(bool self_managed) const -> std::unique_ptr<Panel> {
  auto panel = std::make_unique<Panel>(childPanel());
  if (self_managed) {
    panel->should_self_delete = true;
  }
  return panel;
}

auto Panel::radioButton() const -> std::unique_ptr<RadioButton> {
  return std::make_unique<RadioButton>(new swx::RadioButton(
      _wx(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxRB_SINGLE));
}

auto Panel::scrollingPanel(long scroll_style) const
    -> std::unique_ptr<ScrollingPanel> {
  return std::make_unique<ScrollingPanel>(new swx::ScrolledWindow(
      _wx(), wxID_ANY, wxDefaultPosition, wxDefaultSize, scroll_style));
}

auto Panel::searchBox(const std::string &initial_text) const
    -> std::unique_ptr<SearchBox> {
  auto search_box = std::make_unique<SearchBox>(
      new swx::SearchCtrl(_wx(), wxID_ANY), initial_text);
  search_box->showSearchButton(false);
  search_box->showCancelButton(true);
  return search_box;
}

auto Panel::text(const std::string &initial_text, bool multi_line) const
    -> std::unique_ptr<Text> {
  if (multi_line) {
    return std::make_unique<Text>(
        new swx::TextCtrl(_wx(), wxID_ANY, initial_text, wxDefaultPosition,
                          wxSize(-1, -1), wxTE_MULTILINE));
  }
  return std::make_unique<Text>(
      new swx::TextCtrl(_wx(), wxID_ANY, initial_text));
}

auto Panel::toggle(const std::string &label) const -> std::unique_ptr<Toggle> {
  return std::make_unique<Toggle>(
      new swx::ToggleButton(_wx(), wxID_ANY, label));
}

}  // namespace cszb_scoreboard
