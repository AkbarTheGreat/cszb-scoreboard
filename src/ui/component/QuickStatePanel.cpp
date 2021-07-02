/*
ui/component/QuickStatePanel.cpp: This panel provides quick-settable screens.

Copyright 2020-2021 Tracy Beck

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
#include "ui/frame/FrameManager.h"
#include "ui/frame/HotkeyTable.h"
#include "ui/frame/MainView.h"
#include "ui/widget/swx/Panel.h"
#include "util/ProtoUtil.h"
#include "wx/gbsizer.h"

namespace cszb_scoreboard {

const int PREVIEW_WIDTH = 85;  // a thumbnail-sized 4x3 display
const int PREVIEW_HEIGHT = 64;
// If this is increased over 10 without updating hotkey logic, bad things may
// happen.
const int NUMBER_OF_QUICK_PANELS = 10;

QuickStateEntry::QuickStateEntry(swx::Panel* wx, int id) : ScreenText(wx) {
  setupPreview("", {ProtoUtil::homeSide(), ProtoUtil::awaySide()},
               wxSize(PREVIEW_WIDTH, PREVIEW_HEIGHT));

  setAllText("", 1, Color("Gray"), true, ProtoUtil::homeSide());
  setAllText("", 1, Color("Gray"), true, ProtoUtil::awaySide());

  // These two buttons are always hidden and exist only to add hotkey support
  set_button = new wxButton(wx, wxID_ANY);
  execute_button = new wxButton(wx, wxID_ANY);
  set_button->Hide();
  execute_button->Hide();

  bindEvents(id);
}

QuickStateEntry::~QuickStateEntry() { screen()->Destroy(); }

void QuickStateEntry::bindEvents(int id) {
  auto* parent = dynamic_cast<QuickStatePanel*>(screen()->GetParent());

  char command_button = '1' + id;
  if (id >= 9) {  // NOLINT(readability-magic-numbers) 9 is the highest single
                  // digit number.
    command_button = '0';
  }
  std::string tooltip = tooltipText(command_button);

  for (auto* side : screen()->sides()) {
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
  auto* parent = dynamic_cast<QuickStatePanel*>(screen()->GetParent());
  parent->executeShortcut(screen());
}

void QuickStateEntry::setShortcut() {
  initialized = true;
  auto* parent = dynamic_cast<QuickStatePanel*>(screen()->GetParent());
  parent->setShortcut(screen());
}

auto QuickStateEntry::tooltipText(char command_character) -> std::string {
  std::string string_template =
      "Right Click (Ctrl+Alt+%c) to set\nLeft Click (Ctrl+%c) to send to "
      "monitors";
  std::string buffer;

  size_t size = string_template.length() + 2;
  buffer.reserve(size + 1);
  buffer.resize(size);

  snprintf(&buffer[0], size + 1, string_template.c_str(), command_character,
           command_character);
  return buffer;
}

QuickStatePanel::QuickStatePanel(wxWindow* parent) : wxPanel(parent) {
  for (int i = 0; i < NUMBER_OF_QUICK_PANELS; ++i) {
    entries.push_back(std::move(std::make_unique<QuickStateEntry>(new swx::Panel(this), i)));
  }
  positionWidgets();
}

void QuickStatePanel::positionWidgets() {
  auto* sizer = new wxGridBagSizer();

  for (int i = 0; i < entries.size(); i++) {
    entries[i]->screen()->addToSizer(sizer, i, 0);
  }

  SetSizerAndFit(sizer);
}

void QuickStatePanel::executeShortcut(ScreenText* screen) {
  FrameManager::getInstance()->mainView()->previewPanel()->setToPresenters(
      screen);
}

void QuickStatePanel::setShortcut(ScreenText* screen) {
  FrameManager::getInstance()
      ->mainView()
      ->controlPanel()
      ->updateScreenTextFromSelected(screen);
}

}  // namespace cszb_scoreboard
