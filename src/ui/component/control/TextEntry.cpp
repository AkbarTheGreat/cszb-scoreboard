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

#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;
const int BORDER_SIZE = 5;

TextEntry *TextEntry::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  TextEntry *entry = new TextEntry(preview_panel, parent);
  entry->initializeWidgets();
  entry->updatePreview();
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

  inner_panel = new wxPanel(control_panel);

  font_size_label = new wxStaticText(inner_panel, wxID_ANY, wxT("Font Size"));
  font_size_entry = new wxTextCtrl(inner_panel, wxID_ANY,
                                   StringUtil::intToString(DEFAULT_FONT_SIZE));

  int number_of_screen_choices = sizeof(screen_choices) / sizeof(wxString);
  screen_selection = new wxRadioBox(
      inner_panel, wxID_ANY, wxT("Team"), wxDefaultPosition, wxDefaultSize,
      number_of_screen_choices, screen_choices, 1, wxRA_SPECIFY_COLS);
  screen_selection->SetSelection(0);

  positionWidgets(control_panel);
  bindEvents();
}

void TextEntry::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *outer_sizer = new wxFlexGridSizer(0, 3, 0, 0);
  outer_sizer->SetFlexibleDirection(wxBOTH);
  outer_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  wxFlexGridSizer *inner_sizer = new wxFlexGridSizer(0, 2, 0, 0);
  inner_sizer->SetFlexibleDirection(wxBOTH);
  inner_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  // Outer sizer holds text label and inner_panel
  outer_sizer->Add(text_label, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(text_entry, 0, wxALL, BORDER_SIZE);
  outer_sizer->Add(inner_panel, 0, wxALL, BORDER_SIZE);

  inner_sizer->Add(font_size_label, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(font_size_entry, 0, wxALL, BORDER_SIZE);
  inner_sizer->Add(new wxStaticText(inner_panel, -1, wxT("")), 0, wxALL,
                   BORDER_SIZE);
  inner_sizer->Add(screen_selection, 0, wxALL, BORDER_SIZE);

  inner_panel->SetSizerAndFit(inner_sizer);
  control_panel->SetSizerAndFit(outer_sizer);
}

void TextEntry::bindEvents() {
  text_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  font_size_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &TextEntry::screenChanged, this);
}

wxTextCtrl *TextEntry::textField() { return text_entry; }

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
      font_size_entry->SetValue(StringUtil::intToString(home_font_size));
      break;
    case 1:
      text_entry->SetValue(away_text);
      font_size_entry->SetValue(StringUtil::intToString(away_font_size));
      break;
    default:
      text_entry->SetValue(all_text);
      font_size_entry->SetValue(StringUtil::intToString(all_font_size));
      break;
  }
  updatePreview();
}

int TextEntry::enteredFontSize() {
  return (int)StringUtil::stringToInt(font_size_entry->GetValue(),
                                      DEFAULT_FONT_SIZE);
}

}  // namespace cszb_scoreboard