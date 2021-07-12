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

#include <stddef.h>
#include <wx/accel.h>
#include <wx/defs.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/gtk/button.h>
#include <wx/wxcrtvararg.h>
#include <string>
#include <algorithm>
#include <utility>

#include "ui/frame/FrameManager.h"
#include "ui/frame/HotkeyTable.h"
#include "ui/frame/MainView.h"
#include "ui/widget/swx/Panel.h"
#include "util/ProtoUtil.h"
#include "wx/gbsizer.h"
#include "config.pb.h"
#include "ui/component/ControlPanel.h"
#include "ui/component/PreviewPanel.h"
#include "ui/component/ScreenTextSide.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {

const int PREVIEW_WIDTH = 85;  // a thumbnail-sized 4x3 display
const int PREVIEW_HEIGHT = 64;
// If this is increased over 10 without updating hotkey logic, bad things may
// happen.
const int NUMBER_OF_QUICK_PANELS = 10;

QuickStateEntry::QuickStateEntry(swx::Panel* wx, QuickStatePanel* parent,
                                 int id)
    : ScreenText(wx) {
  this->parent = parent;
  should_self_delete = true;
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

void QuickStateEntry::bindEvents(int id) {
  char command_button = '1' + id;
  if (id >= 9) {  // NOLINT(readability-magic-numbers) 9 is the highest single
                  // digit number.
    command_button = '0';
  }
  std::string tooltip = tooltipText(command_button);

  for (auto* side : sides()) {
    // You have to bind events directly to the ScreenTextSide, as mouse events
    // don't propagate up to parent widgets (even if the child widget doesn't
    // have a handler bound for that event, apparently.)
    side->bind(wxEVT_RIGHT_UP, [this](wxMouseEvent& event) -> void {
      this->setShortcutFromPanel(event);
    });
    side->bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) -> void {
      this->executeShortcutFromPanel(event);
    });
    side->toolTip(tooltip);
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
  parent->executeShortcut(this);
}

void QuickStateEntry::setShortcut() {
  initialized = true;
  parent->setShortcut(this);
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

QuickStatePanel::QuickStatePanel(swx::Panel* wx) : Panel(wx) {
  for (int i = 0; i < NUMBER_OF_QUICK_PANELS; ++i) {
    entries.push_back(
        std::move(std::make_unique<QuickStateEntry>(childPanel(), this, i)));
  }
  positionWidgets();
}

void QuickStatePanel::positionWidgets() {
  auto* sizer = new wxGridBagSizer();

  for (int i = 0; i < entries.size(); i++) {
    addWidget(*entries[i], i, 0);
  }

  runSizer();
}

void QuickStatePanel::executeShortcut(QuickStateEntry* entry) {
  FrameManager::getInstance()->mainView()->previewPanel()->setToPresenters(
      entry);
}

void QuickStatePanel::setShortcut(QuickStateEntry* entry) {
  FrameManager::getInstance()
      ->mainView()
      ->controlPanel()
      ->updateScreenTextFromSelected(entry);
}

}  // namespace cszb_scoreboard
