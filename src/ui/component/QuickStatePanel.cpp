/*
ui/component/QuickStatePanel.cpp: This panel provides quick-settable screens.

Copyright 2020 Tracy Beck

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

#include "ui/component/QuickStatePanel.h"

#include <string>

#include "ui/UiUtil.h"
#include "ui/frame/HotkeyTable.h"
#include "ui/frame/MainView.h"
#include "util/ProtoUtil.h"
#include "wx/gbsizer.h"

namespace cszb_scoreboard {

const int PREVIEW_WIDTH = 85;  // a thumbnail-sized 4x3 display
const int PREVIEW_HEIGHT = 64;

QuickStateEntry::QuickStateEntry(wxPanel* parent, int id) {
  screen_text = ScreenText::getPreview(
      parent, "", {ProtoUtil::homeSide(), ProtoUtil::awaySide()},
      wxSize(PREVIEW_WIDTH, PREVIEW_HEIGHT));

  screen_text->setAllText("", 1, Color("Gray"), true, ProtoUtil::homeSide());
  screen_text->setAllText("", 1, Color("Gray"), true, ProtoUtil::awaySide());

  // These two buttons are always hidden and exist only to add hotkey support
  set_button = new wxButton(parent, wxID_ANY);
  execute_button = new wxButton(parent, wxID_ANY);
  set_button->Hide();
  execute_button->Hide();

  bindEvents(id);
}

void QuickStateEntry::bindEvents(int id) {
  QuickStatePanel* parent = (QuickStatePanel*)screen()->GetParent();

  char command_button = 0x31 + id;
  if (id >= 9) {
    command_button = 0x30;
  }
  std::string tooltip = tooltipText(command_button);

  for (auto side : screen()->sides()) {
    // You have to bind events directly to the ScreenTextSide, as mouse events
    // don't propagate up to parent widgets (even if the child widget doesn't
    // have a handler bound for that event, apparently.)
    side->Bind(wxEVT_RIGHT_UP, &QuickStateEntry::setShortcutFromPanel, this);
    side->Bind(wxEVT_LEFT_UP, &QuickStateEntry::executeShortcutFromPanel, this);
    side->SetToolTip(tooltip);
  }

  execute_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                       &QuickStateEntry::executeShortcutFromButton, this);
  set_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                   &QuickStateEntry::setShortcutFromButton, this);

  HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL, command_button,
                                        execute_button->GetId());
  HotkeyTable::getInstance()->addHotkey(wxACCEL_CTRL | wxACCEL_ALT,
                                        command_button, set_button->GetId());
}

void QuickStateEntry::executeShortcut() {
  if (!initialized) {
    return;
  }
  QuickStatePanel* parent = (QuickStatePanel*)screen()->GetParent();
  parent->executeShortcut(screen());
}

void QuickStateEntry::setShortcut() {
  initialized = true;
  QuickStatePanel* parent = (QuickStatePanel*)screen()->GetParent();
  parent->setShortcut(screen());
}

std::string QuickStateEntry::tooltipText(char command_character) {
  std::unique_ptr<char[]> buffer(new char[100]);
  int size = snprintf(buffer.get(), 100,
                      "Right Click (Ctrl+Alt+%c) to set\nLeft Click (Ctrl+%c) "
                      "to send to monitors",
                      command_character, command_character);
  return std::string(buffer.get(), buffer.get() + size);
}

QuickStatePanel::QuickStatePanel(wxWindow* parent) : wxPanel(parent) {
  for (int i = 0; i < 10; ++i) {
    entries.push_back(new QuickStateEntry(this, i));
  }
  positionWidgets();
}

QuickStatePanel::~QuickStatePanel() {
  for (auto entry : entries) {
    entry->screen()->Destroy();
  }

  entries.clear();
}

void QuickStatePanel::positionWidgets() {
  wxGridBagSizer* sizer = new wxGridBagSizer();

  for (int i = 0; i < entries.size(); i++) {
    UiUtil::addToGridBag(sizer, entries[i]->screen(), i, 0);
  }

  SetSizerAndFit(sizer);
}

void QuickStatePanel::executeShortcut(ScreenText* screen) {
  MainView* main = (MainView*)GetParent();
  main->previewPanel()->setToPresenters(screen);
}

void QuickStatePanel::setShortcut(ScreenText* screen) {
  MainView* main = (MainView*)GetParent();
  main->controlPanel()->updateScreenTextFromSelected(screen);
}

}  // namespace cszb_scoreboard
