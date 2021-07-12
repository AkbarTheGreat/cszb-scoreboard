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
    -> std::unique_ptr<TextEntry> {
  auto entry = std::make_unique<TextEntry>(preview_panel, wx);
  entry->initializeWidgets();
  entry->updatePreview();
  return entry;
}

TextEntry::TextEntry(PreviewPanel *preview_panel, swx::Panel *wx)
    : ScreenTextController(preview_panel, wx) {
  home_text = "Home Team";
  away_text = "Away Team";
  all_text = "Enter Text";

  home_font_size = away_font_size = all_font_size = DEFAULT_FONT_SIZE;

  home_color = TeamConfig::getInstance()->teamColor(ProtoUtil::homeSide())[0];
  away_color = TeamConfig::getInstance()->teamColor(ProtoUtil::awaySide())[0];

  all_color = Color("Black");
}

void TextEntry::createControls(Panel *control_panel) {
  text_label = control_panel->label("Text");
  text_entry = control_panel->text(all_text, true);

  inner_panel = control_panel->panel();

  font_size_label = inner_panel->label("Font Size");
  font_size_entry =
      inner_panel->text(std::to_string(DEFAULT_FONT_SIZE));

  screen_selection = std::make_unique<TeamSelector>(inner_panel->childPanel(),
                                                    ProtoUtil::allSide());

  color_picker = inner_panel->colorPicker(all_color);

  positionWidgets(control_panel);
  bindEvents();
}

void TextEntry::positionWidgets(Panel *control_panel) {
  // wxSizer *inner_sizer = UiUtil::sizer(0, 2);

  control_panel->addWidget(*text_label, 0, 0);
  control_panel->addWidget(*text_entry, 0, 1);
  control_panel->addWidget(*inner_panel, 0, 2);

  inner_panel->addWidget(*font_size_label, 0, 0);
  inner_panel->addWidget(*font_size_entry, 0, 1);
  inner_panel->addWidget(*screen_selection, 1, 0);
  inner_panel->addWidget(*color_picker, 1, 1);

  inner_panel->runSizer();
  control_panel->runSizer();
}

void TextEntry::bindEvents() {
  text_entry->bind(wxEVT_KEY_UP,
                   [this](wxKeyEvent &event) -> void { this->textUpdated(); });
  font_size_entry->bind(
      wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void { this->textUpdated(); });
  screen_selection->bind(
      wxEVT_COMMAND_RADIOBOX_SELECTED,
      [this](wxCommandEvent &event) -> void { this->screenChanged(); });
  color_picker->bind(wxEVT_COLOURPICKER_CHANGED,
                     [this](wxColourPickerEvent &event) -> void {
                       this->colorChanged(event);
                     });
}

auto TextEntry::textField() -> Text * { return text_entry.get(); }

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

void TextEntry::colorChanged(const wxColourPickerEvent &event) {
  if (screen_selection->allSelected()) {
    all_color = Color(event.GetColour());
  } else if (screen_selection->homeSelected()) {
    home_color = Color(event.GetColour());
  } else if (screen_selection->awaySelected()) {
    away_color = Color(event.GetColour());
  }
  updatePreview();
}

void TextEntry::textUpdated() {
  if (screen_selection->allSelected()) {
    all_text = text_entry->value();
    all_font_size = enteredFontSize();
  } else if (screen_selection->homeSelected()) {
    home_text = text_entry->value();
    home_font_size = enteredFontSize();
  } else if (screen_selection->awaySelected()) {
    away_text = text_entry->value();
    away_font_size = enteredFontSize();
  }
  updatePreview();
}

void TextEntry::selectTeam(int index) {
  screen_selection->setSelection(index);
  screenChanged();
}

void TextEntry::screenChanged() {
  if (screen_selection->allSelected()) {
    text_entry->setValue(all_text);
    font_size_entry->setValue(all_font_size);
    color_picker->setColor(all_color);
  } else if (screen_selection->homeSelected()) {
    text_entry->setValue(home_text);
    font_size_entry->setValue(home_font_size);
    color_picker->setColor(home_color);
  } else if (screen_selection->awaySelected()) {
    text_entry->setValue(away_text);
    font_size_entry->setValue(away_font_size);
    color_picker->setColor(away_color);
  }

  updatePreview();
}

auto TextEntry::enteredFontSize() -> int {
  return static_cast<int>(
      StringUtil::stringToInt(font_size_entry->value(), DEFAULT_FONT_SIZE));
}

}  // namespace cszb_scoreboard
