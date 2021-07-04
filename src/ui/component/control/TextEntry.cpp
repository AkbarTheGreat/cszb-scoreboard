/*
ui/component/control/TextEntry.cpp: This class is responsible for generating
text which can go to one or all of the scoreboard screens.

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

#include "ui/component/control/TextEntry.h"

#include "config/TeamConfig.h"
#include "ui/UiUtil.h"
#include "util/ProtoUtil.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;
const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

auto TextEntry::Create(PreviewPanel *preview_panel, swx::Panel *wx)
    -> TextEntry * {
  auto *entry = new TextEntry(preview_panel, wx);
  entry->initializeWidgets();
  entry->updatePreview();
  return entry;
}

TextEntry::TextEntry(PreviewPanel *preview_panel, swx::Panel *wx)
    : ScreenTextController(preview_panel, wx) {
  home_text = wxT("Home Team");
  away_text = wxT("Away Team");
  all_text = wxT("Enter Text");

  home_font_size = away_font_size = all_font_size = DEFAULT_FONT_SIZE;

  home_color = TeamConfig::getInstance()->teamColor(ProtoUtil::homeSide())[0];
  away_color = TeamConfig::getInstance()->teamColor(ProtoUtil::awaySide())[0];

  all_color = Color("Black");
}

void TextEntry::createControls(wxPanel *control_panel) {
  text_label = new wxStaticText(control_panel, wxID_ANY, wxT("Text"));
  text_entry =
      new wxTextCtrl(control_panel, wxID_ANY, all_text, wxDefaultPosition,
                     wxSize(-1, -1), wxTE_MULTILINE);

  inner_panel = new wxPanel(control_panel);

  font_size_label = new wxStaticText(inner_panel, wxID_ANY, wxT("Font Size"));
  font_size_entry = new wxTextCtrl(inner_panel, wxID_ANY,
                                   StringUtil::intToString(DEFAULT_FONT_SIZE));

  screen_selection = new TeamSelector(inner_panel, ProtoUtil::allSide());

  color_picker = new wxColourPickerCtrl(inner_panel, wxID_ANY, all_color);

  positionWidgets(control_panel);
  bindEvents();
}

void TextEntry::positionWidgets(wxPanel *control_panel) {
  wxSizer *outer_sizer = UiUtil::sizer(0, 3);
  wxSizer *inner_sizer = UiUtil::sizer(0, 2);

  // Outer sizer holds text label and inner_panel
  outer_sizer->Add(text_label, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(text_entry, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(inner_panel, 0, wxALL, BORDER_SIZE);

  inner_sizer->Add(font_size_label, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(font_size_entry, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(color_picker, 0, wxALL, BORDER_SIZE);

  inner_panel->SetSizerAndFit(inner_sizer);
  control_panel->SetSizerAndFit(outer_sizer);
}

void TextEntry::bindEvents() {
  text_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  font_size_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &TextEntry::screenChanged, this);
  color_picker->Bind(wxEVT_COLOURPICKER_CHANGED, &TextEntry::colorChanged,
                     this);
}

auto TextEntry::textField() -> wxTextCtrl * { return text_entry; }

void TextEntry::updateScreenText(ScreenText *screen_text) {
  // Send the combined text to both previews
  if (screen_selection->allSelected()) {
    screen_text->setAllText(all_text, all_font_size, all_color, true,
                            ProtoUtil::allSide());
  } else {
    screen_text->setAllText(home_text, home_font_size, home_color, true,
                            ProtoUtil::homeSide());
    screen_text->setAllText(away_text, away_font_size, away_color, true,
                            ProtoUtil::awaySide());
  }
}

void TextEntry::colorChanged(wxColourPickerEvent &event) {
  if (screen_selection->allSelected()) {
    all_color = Color(event.GetColour());
  } else if (screen_selection->homeSelected()) {
    home_color = Color(event.GetColour());
  } else if (screen_selection->awaySelected()) {
    away_color = Color(event.GetColour());
  }
  updatePreview();
}

void TextEntry::textUpdated(wxKeyEvent &event) {
  if (screen_selection->allSelected()) {
    all_text = text_entry->GetValue();
    all_font_size = enteredFontSize();
  } else if (screen_selection->homeSelected()) {
    home_text = text_entry->GetValue();
    home_font_size = enteredFontSize();
  } else if (screen_selection->awaySelected()) {
    away_text = text_entry->GetValue();
    away_font_size = enteredFontSize();
  }
  updatePreview();
}

void TextEntry::selectTeam(int index) {
  screen_selection->SetSelection(index);
  doScreenChanged();
}

void TextEntry::screenChanged(wxCommandEvent &event) { doScreenChanged(); }

void TextEntry::doScreenChanged() {
  if (screen_selection->allSelected()) {
    text_entry->SetValue(all_text);
    font_size_entry->SetValue(StringUtil::intToString(all_font_size));
    color_picker->SetColour(all_color);
  } else if (screen_selection->homeSelected()) {
    text_entry->SetValue(home_text);
    font_size_entry->SetValue(StringUtil::intToString(home_font_size));
    color_picker->SetColour(home_color);
  } else if (screen_selection->awaySelected()) {
    text_entry->SetValue(away_text);
    font_size_entry->SetValue(StringUtil::intToString(away_font_size));
    color_picker->SetColour(away_color);
  }

  updatePreview();
}

auto TextEntry::enteredFontSize() -> int {
  return static_cast<int>(
      StringUtil::stringToInt(font_size_entry->GetValue(), DEFAULT_FONT_SIZE));
}

}  // namespace cszb_scoreboard
