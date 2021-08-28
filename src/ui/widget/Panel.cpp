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

#include <wx/anybutton.h>  // for wxBU_EXACTFIT
#include <wx/filedlg.h>    // for wxFD_FILE_MUST_EXIST
#include <wx/textctrl.h>   // for wxTE_MULTILINE

#include <vector>  // for vector

#include "ui/widget/Button.h"                // for Button
#include "ui/widget/CheckBox.h"              // for CheckBox
#include "ui/widget/ColorPicker.h"           // for ColorPicker
#include "ui/widget/Divider.h"               // for Divider
#include "ui/widget/FilePicker.h"            // for FilePicker
#include "ui/widget/Label.h"                 // for Label
#include "ui/widget/ListBox.h"               // for ListBox
#include "ui/widget/Radio.h"                 // for Radio
#include "ui/widget/RadioButton.h"           // for RadioButton
#include "ui/widget/ScrollingPanel.h"        // for ScrollingPanel
#include "ui/widget/SearchBox.h"             // for SearchBox
#include "ui/widget/Text.h"                  // for Text
#include "ui/widget/Toggle.h"                // for Toggle
#include "ui/widget/swx/Button.h"            // for Button
#include "ui/widget/swx/CheckBox.h"          // for CheckBox
#include "ui/widget/swx/ColourPickerCtrl.h"  // for ColourPickerCtrl
#include "ui/widget/swx/EditableListBox.h"   // for EditableListBox
#include "ui/widget/swx/FileDialog.h"        // for FileDialog
#include "ui/widget/swx/RadioBox.h"          // for RadioBox
#include "ui/widget/swx/RadioButton.h"       // for RadioButton
#include "ui/widget/swx/ScrolledWindow.h"    // for ScrolledWindow
#include "ui/widget/swx/SearchCtrl.h"        // for SearchCtrl
#include "ui/widget/swx/StaticLine.h"        // for StaticLine
#include "ui/widget/swx/StaticText.h"        // for StaticText
#include "ui/widget/swx/TextCtrl.h"          // for TextCtrl
#include "ui/widget/swx/ToggleButton.h"      // for ToggleButton

namespace cszb_scoreboard {

Panel::~Panel() {
  if (should_self_delete) {
    _wx()->Destroy();
  }
}

auto Panel::size() const -> Size { return Size::fromWx(_wx()->GetSize()); }

auto Panel::childPanel() const -> swx::Panel * { return new swx::Panel(_wx()); }

auto Panel::childPanel(int width, int height) const -> swx::Panel * {
  return new swx::Panel(_wx(), wxID_ANY, wxDefaultPosition,
                        wxSize(width, height));
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

// Get a FilePicker to open an existing file.
auto Panel::openFilePicker(const std::string &title,
                           const std::string &selectionMode) const
    -> std::unique_ptr<FilePicker> {
  return std::make_unique<FilePicker>(new swx::FileDialog(
      wx, title, "", "", selectionMode, wxFD_OPEN | wxFD_FILE_MUST_EXIST));
}

auto Panel::label(const std::string &text) const -> std::unique_ptr<Label> {
  return std::make_unique<Label>(new swx::StaticText(_wx(), wxID_ANY, text));
}

auto Panel::listBox(const std::string &title) const
    -> std::unique_ptr<ListBox> {
  return std::make_unique<ListBox>(
      new swx::EditableListBox(_wx(), wxID_ANY, title));
}

auto Panel::panel(bool self_managed) const -> std::unique_ptr<Panel> {
  auto panel = std::make_unique<Panel>(childPanel());
  if (self_managed) {
    panel->should_self_delete = true;
  }
  return panel;
}

auto Panel::radio(const std::string &name, const char *const *choices,
                  int64_t choices_size, bool is_vertical) const
    -> std::unique_ptr<Radio> {
  int64_t style = wxRA_SPECIFY_COLS;
  if (is_vertical) {
    style = wxRA_SPECIFY_ROWS;
  }
  std::vector<wxString> wx_choices(choices_size);
  for (int i = 0; i < choices_size; i++) {
    wx_choices[i] = choices[i];
  }
  return std::make_unique<Radio>(
      new swx::RadioBox(_wx(), wxID_ANY, name, wxDefaultPosition, wxDefaultSize,
                        wx_choices.size(), wx_choices.data(), 0, style));
}

auto Panel::radioButton() const -> std::unique_ptr<RadioButton> {
  return std::make_unique<RadioButton>(new swx::RadioButton(
      _wx(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxRB_SINGLE));
}

auto Panel::scrollingPanel() const -> std::unique_ptr<ScrollingPanel> {
  return std::make_unique<ScrollingPanel>(new swx::ScrolledWindow(
      _wx(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL));
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
