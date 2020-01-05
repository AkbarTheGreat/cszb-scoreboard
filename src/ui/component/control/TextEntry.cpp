/*
ui/component/control/TextEntry.cpp: This class is responsible for generating
text which can go to one or all of the scoreboard screens.

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

#include "ui/component/control/TextEntry.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;

TextEntry *TextEntry::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  TextEntry *entry = new TextEntry(preview_panel, parent);
  entry->initializeWidgets();
  return entry;
}

void TextEntry::createControls(wxPanel *control_panel) {
  home_text = wxT("Home Team");
  away_text = wxT("Away Team");
  all_text = wxT("");
  home_font_size = away_font_size = all_font_size = DEFAULT_FONT_SIZE;

  text_label = new wxStaticText(control_panel, wxID_ANY, wxT("Text"));
  text_entry =
      new wxTextCtrl(control_panel, wxID_ANY, home_text, wxDefaultPosition,
                     wxSize(-1, -1), wxTE_MULTILINE);
  font_size_label = new wxStaticText(control_panel, wxID_ANY, wxT("Font Size"));
  font_size_entry =
      new wxTextCtrl(control_panel, wxID_ANY, intToString(DEFAULT_FONT_SIZE));

  int number_of_screen_choices = sizeof(screen_choices) / sizeof(wxString);
  screen_selection = new wxRadioBox(
      control_panel, wxID_ANY, wxT("Team"), wxDefaultPosition, wxDefaultSize,
      number_of_screen_choices, screen_choices, 1, wxRA_SPECIFY_COLS);
  screen_selection->SetSelection(0);

  positionWidgets(control_panel);
  bindEvents();
}

void TextEntry::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 4, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  sizer->Add(text_label, 0, wxALL, 5);
  sizer->Add(text_entry, 0, wxALL, 5);
  sizer->Add(font_size_label, 0, wxALL, 5);
  sizer->Add(font_size_entry, 0, wxALL, 5);
  sizer->Add(screen_selection, 0, wxALL, 5);
  control_panel->SetSizerAndFit(sizer);
}

void TextEntry::bindEvents() {
  text_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  font_size_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &TextEntry::screenChanged, this);
}

wxTextCtrl *TextEntry::textField() { return text_entry; }

proto::ScreenSide TextEntry::updateSide() {
  proto::ScreenSide side;
  switch (screen_selection->GetSelection()) {
    case 0:
      side.set_home(true);
      break;
    case 1:
      side.set_away(true);
      break;
    default:
      side.set_home(true);
      side.set_away(true);
      break;
  }
  return side;
}

void TextEntry::updatePreview() {
  // Send the combined text to both previews
  if (screen_selection->GetSelection() == 2) {
    proto::ScreenSide side;
    side.set_home(true);
    side.set_away(true);
    previewPanel()->setTextForPreview(all_text, all_font_size, side);
  } else {
    proto::ScreenSide home_side;
    home_side.set_home(true);
    proto::ScreenSide away_side;
    away_side.set_away(true);
    previewPanel()->setTextForPreview(home_text, home_font_size, home_side);
    previewPanel()->setTextForPreview(away_text, away_font_size, away_side);
  }
}

void TextEntry::textUpdated(wxKeyEvent &event) {
  switch (screen_selection->GetSelection()) {
    case 0:
      home_text = text_entry->GetValue();
      home_font_size = enteredFontSize();
      break;
    case 1:
      away_text = text_entry->GetValue();
      away_font_size = enteredFontSize();
      break;
    default:
      all_text = text_entry->GetValue();
      all_font_size = enteredFontSize();
      break;
  }
  updatePreview();
}

void TextEntry::screenChanged(wxCommandEvent &event) {
  switch (screen_selection->GetSelection()) {
    case 0:
      text_entry->SetValue(home_text);
      font_size_entry->SetValue(intToString(home_font_size));
      break;
    case 1:
      text_entry->SetValue(away_text);
      font_size_entry->SetValue(intToString(away_font_size));
      break;
    default:
      text_entry->SetValue(all_text);
      font_size_entry->SetValue(intToString(all_font_size));
      break;
  }
  updatePreview();
}

int TextEntry::enteredFontSize() {
  long font_size = DEFAULT_FONT_SIZE;
  if (font_size_entry->GetValue().IsNumber()) {
    font_size_entry->GetValue().ToLong(&font_size);
  }
  return (int)font_size;
}

wxString TextEntry::intToString(int value) {
  wxString string;
  string.Printf(wxT("%d"), value);
  return string;
}

}  // namespace cszb_scoreboard
