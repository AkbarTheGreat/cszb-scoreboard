/*
ui/widget/Panel.cpp: A panel represents an interior panel container that is held
within other frames or panels.

Copyright 2021-2023 Tracy Beck

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
#include <wx/dirdlg.h>     // for wxDD_DIR_MUST_EXIST
#include <wx/filedlg.h>    // for wxFD_FILE_MUST_EXIST

#include <vector>  // for vector

#include "ui/widget/Browser.h"
#include "ui/widget/Button.h"       // for Button
#include "ui/widget/CheckBox.h"     // for CheckBox
#include "ui/widget/ColorPicker.h"  // for ColorPicker
#include "ui/widget/DirectoryPicker.h"
#include "ui/widget/Divider.h"     // for Divider
#include "ui/widget/FilePicker.h"  // for FilePicker
#include "ui/widget/Label.h"       // for Label
#include "ui/widget/LabelledArea.h"
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
#include "ui/widget/swx/DirDialog.h"         // for DirDialog
#include "ui/widget/swx/EditableListBox.h"   // for EditableListBox
#include "ui/widget/swx/FileDialog.h"        // for FileDialog
#include "ui/widget/swx/RadioBox.h"          // for RadioBox
#include "ui/widget/swx/RadioButton.h"       // for RadioButton
#include "ui/widget/swx/ScrolledWindow.h"    // for ScrolledWindow
#include "ui/widget/swx/SearchCtrl.h"        // for SearchCtrl
#include "ui/widget/swx/StaticBox.h"
#include "ui/widget/swx/StaticLine.h"    // for StaticLine
#include "ui/widget/swx/StaticText.h"    // for StaticText
#include "ui/widget/swx/TextCtrl.h"      // for TextCtrl
#include "ui/widget/swx/ToggleButton.h"  // for ToggleButton
#include "ui/widget/swx/WebView.h"
#include "util/FilesystemPath.h"
// IWYU pragma: no_include <ext/alloc_traits.h>

namespace cszb_scoreboard {

Panel::Panel(swx::Panel* panel) { _wx = panel; }

Panel::~Panel() {
  if (should_self_delete) {
    wx()->Destroy();
  }
}

auto Panel::size() const -> Size { return Size::fromWx(wx()->GetSize()); }

auto Panel::childPanel() const -> swx::Panel* {
  return new swx::PanelImpl(wx());
}

auto Panel::childPanel(int width, int height) const -> swx::Panel* {
  return new swx::PanelImpl(wx(), wxID_ANY, wxDefaultPosition,
                            wxSize(width, height));
}

auto Panel::button(const std::string& label, bool exact_fit) const
    -> std::unique_ptr<Button> {
  if (exact_fit) {
    return std::make_unique<Button>(
        new swx::Button(wx(), wxID_ANY, label, wxDefaultPosition, wxDefaultSize,
                        wxBU_EXACTFIT));
  }
  return std::make_unique<Button>(new swx::Button(wx(), wxID_ANY, label));
}

auto Panel::browser(const std::string& url) const -> std::unique_ptr<Browser> {
  return std::make_unique<Browser>(new swx::WebView(wx(), url));
}

auto Panel::checkBox(const std::string& label) const
    -> std::unique_ptr<CheckBox> {
  return std::make_unique<CheckBox>(new swx::CheckBox(wx(), wxID_ANY, label));
}

auto Panel::colorPicker(const wxColour& initial_color) const
    -> std::unique_ptr<ColorPicker> {
  return std::make_unique<ColorPicker>(
      new swx::ColourPickerCtrl(wx(), wxID_ANY, initial_color));
}

auto Panel::divider() const -> std::unique_ptr<Divider> {
  return std::make_unique<Divider>(new swx::StaticLine(wx()));
}

// Get a DirectoryPicker to open an existing path.
auto Panel::openDirectoryPicker(const std::string& title,
                                const FilesystemPath& initial_dir) const
    -> std::unique_ptr<DirectoryPicker> {
  return std::make_unique<DirectoryPicker>(new swx::DirDialog(
      wx(), title, initial_dir.string(), wxDD_DIR_MUST_EXIST));
}

// Get a FilePicker to open an existing file.
auto Panel::openFilePicker(const std::string& title,
                           const std::string& selectionMode) const
    -> std::unique_ptr<FilePicker> {
  return std::make_unique<FilePicker>(new swx::FileDialog(
      wx(), title, "", "", selectionMode, wxFD_OPEN | wxFD_FILE_MUST_EXIST));
}

auto Panel::label(const std::string& text) const -> std::unique_ptr<Label> {
  return std::make_unique<Label>(new swx::StaticText(wx(), wxID_ANY, text));
}
auto Panel::labelledArea(const std::string& label) const
    -> std::unique_ptr<LabelledArea> {
  return std::make_unique<LabelledArea>(
      new swx::StaticBoxImpl(wx(), wxID_ANY, label));
}

auto Panel::listBox(const std::string& title) const
    -> std::unique_ptr<ListBox> {
  return std::make_unique<ListBox>(
      new swx::EditableListBox(wx(), wxID_ANY, title));
}

auto Panel::panel(bool self_managed) const -> std::unique_ptr<Panel> {
  auto panel = std::make_unique<Panel>(childPanel());
  if (self_managed) {
    panel->should_self_delete = true;
  }
  return panel;
}

auto Panel::radio(const std::string& name, const char* const* choices,
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
      new swx::RadioBox(wx(), wxID_ANY, name, wxDefaultPosition, wxDefaultSize,
                        wx_choices.size(), wx_choices.data(), 0, style));
}

auto Panel::radioButton() const -> std::unique_ptr<RadioButton> {
  return std::make_unique<RadioButton>(new swx::RadioButton(
      wx(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxRB_SINGLE));
}

auto Panel::scrollingPanel() const -> std::unique_ptr<ScrollingPanel> {
  return std::make_unique<ScrollingPanel>(new swx::ScrolledWindow(
      wx(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL));
}

auto Panel::searchBox(const std::string& initial_text) const
    -> std::unique_ptr<SearchBox> {
  auto search_box = std::make_unique<SearchBox>(
      new swx::SearchCtrl(wx(), wxID_ANY), initial_text);
  search_box->showSearchButton(false);
  search_box->showCancelButton(true);
  return search_box;
}

auto Panel::text(const std::string& initial_text, bool multi_line) const
    -> std::unique_ptr<Text> {
  if (multi_line) {
    return std::make_unique<Text>(
        new swx::TextCtrl(wx(), wxID_ANY, initial_text, wxDefaultPosition,
                          wxSize(-1, -1), wxTE_MULTILINE));
  }
  return std::make_unique<Text>(
      new swx::TextCtrl(wx(), wxID_ANY, initial_text));
}

auto Panel::toggle(const std::string& label) const -> std::unique_ptr<Toggle> {
  return std::make_unique<Toggle>(new swx::ToggleButton(wx(), wxID_ANY, label));
}

void Panel::setBorder(bool has_border) const {
  wx()->SetWindowStyle(has_border ? wxBORDER_SIMPLE : wxBORDER_DEFAULT);
}

}  // namespace cszb_scoreboard
