/*
ui/TextEntry.cpp: This class is responsible for generating text which can go to
one or all of the scoreboard screens.

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

#include "ui/TextEntry.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;

TextEntry::TextEntry(MainView *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxTAB_TRAVERSAL) {
  text_entry = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxSize(-1, -1), wxTE_MULTILINE);
  wxString initial_size;
  initial_size.Printf(wxT("%d"), DEFAULT_FONT_SIZE);

  font_size_entry = new wxTextCtrl(this, wxID_ANY, initial_size,
                                   wxDefaultPosition, wxSize(-1, -1), 0);
  this->parent = parent;

  update_screens = new wxButton(this, wxID_ANY, wxT("Update"),
                                wxDefaultPosition, wxDefaultSize, 0);

  int number_of_screen_choices = sizeof(screen_choices) / sizeof(wxString);
  screen_selection = new wxRadioBox(
      this, wxID_ANY, wxT("Send to Screen"), wxDefaultPosition, wxDefaultSize,
      number_of_screen_choices, screen_choices, 1, wxRA_SPECIFY_COLS);
  screen_selection->SetSelection(0);

  wxFlexGridSizer *sizer = new wxFlexGridSizer(0, 2, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  sizer->Add(text_entry, 0, wxALL, 5);
  sizer->Add(font_size_entry, 0, wxALL, 5);
  sizer->Add(update_screens, 0, wxALL, 5);
  sizer->Add(screen_selection, 0, wxALL, 5);
  SetSizerAndFit(sizer);

  text_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  font_size_entry->Bind(wxEVT_KEY_UP, &TextEntry::textUpdated, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &TextEntry::screenChanged, this);
  update_screens->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TextEntry::updateClicked,
                       this);
}

wxButton *TextEntry::updateButton() { return update_screens; }

wxTextCtrl *TextEntry::textField() { return text_entry; }

proto::ScreenSide TextEntry::selectedSide() {
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

void TextEntry::textUpdated(wxKeyEvent &event) {
  parent->setTextForPreview(text_entry->GetValue(), enteredFontSize(),
                            selectedSide());
}

void TextEntry::screenChanged(wxCommandEvent &event) {
  // For now, if someone changes the screen, just send the text to that screen
  // immediately and leave the old one as-is.  Alternatives for the future:
  // a) Clear the text box at this point.
  // b) Revert the old screen to what it was before the last edit, then send
  // text to the new one.
  textUpdated(wxKeyEvent());
}

void TextEntry::updateClicked(wxCommandEvent &event) {
  parent->updatePresenters(selectedSide());
}

int TextEntry::enteredFontSize() {
  long font_size = DEFAULT_FONT_SIZE;
  if (font_size_entry->GetValue().IsNumber()) {
    font_size_entry->GetValue().ToLong(&font_size);
  }
  return (int)font_size;
}

}  // namespace cszb_scoreboard
