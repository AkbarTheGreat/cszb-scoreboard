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

#include <wx/wxcrtvararg.h>  // for snprintf

#include <cstddef>  // for size_t
#include <string>   // for string
#include <utility>  // for move

#include "config.pb.h"                    // for ScreenSide
#include "config/Position.h"              // for Size
#include "config/swx/event.h"             // for wxEVT_COMMAND_BUTTON_CLICKED
#include "ui/component/ControlPanel.h"    // for ControlPanel
#include "ui/component/PreviewPanel.h"    // for PreviewPanel
#include "ui/component/ScreenTextSide.h"  // for ScreenTextSide
#include "ui/frame/FrameManager.h"        // for FrameManager
#include "ui/frame/HotkeyTable.h"         // for HotkeyTable, wxACCEL_CTRL
#include "ui/frame/MainView.h"            // for MainView
#include "ui/graphics/Color.h"            // for Color
#include "util/ProtoUtil.h"               // for ProtoUtil

namespace cszb_scoreboard {

const int PREVIEW_WIDTH = 85;  // a thumbnail-sized 4x3 display
const int PREVIEW_HEIGHT = 64;
// If this is increased over 10 without updating hotkey logic, bad things may
// happen.
const int NUMBER_OF_QUICK_PANELS = 10;

QuickStateEntry::QuickStateEntry(swx::Panel *wx, int id, Singleton *singleton)
    : ScreenText(wx) {
  this->singleton = singleton;
  setupPreview("", {ProtoUtil::homeSide(), ProtoUtil::awaySide()},
               Size{.width = PREVIEW_WIDTH, .height = PREVIEW_HEIGHT});

  setAllText("", 1, Color("Gray"), true, ProtoUtil::homeSide());
  setAllText("", 1, Color("Gray"), true, ProtoUtil::awaySide());

  // These two buttons are always hidden and exist only to add hotkey support
  set_button = button("");
  execute_button = button("");
  set_button->hide();
  execute_button->hide();

  bindEvents(id);
}

void QuickStateEntry::bindEvents(int id) {
  char command_button = '1' + id;
  if (id >= 9) {  // NOLINT(readability-magic-numbers) 9 is the highest single
                  // digit number.
    command_button = '0';
  }
  std::string tooltip = tooltipText(command_button);

  for (auto *side : sides()) {
    // You have to bind events directly to the ScreenTextSide, as mouse events
    // don't propagate up to parent widgets (even if the child widget doesn't
    // have a handler bound for that event, apparently.)
    side->bind(wxEVT_RIGHT_UP,
               [this](wxMouseEvent &event) -> void { this->setShortcut(); });
    side->bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) -> void {
      this->executeShortcut();
    });
    side->toolTip(tooltip);
  }

  execute_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->executeShortcut(); });
  set_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->executeShortcut(); });

  singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL, command_button,
                                      execute_button->id());
  singleton->hotkeyTable()->addHotkey(wxACCEL_CTRL | wxACCEL_ALT,
                                      command_button, set_button->id());
}

void QuickStateEntry::executeShortcut() {
  if (!initialized) {
    return;
  }
  QuickStatePanel::executeShortcut(this, singleton);
}

void QuickStateEntry::setShortcut() {
  initialized = true;
  QuickStatePanel::setShortcut(this, singleton);
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

QuickStatePanel::QuickStatePanel(swx::Panel *wx, Singleton *singleton)
    : Panel(wx) {
  for (int i = 0; i < NUMBER_OF_QUICK_PANELS; ++i) {
    entries.push_back(std::move(
        std::make_unique<QuickStateEntry>(childPanel(), i, singleton)));
  }
  positionWidgets();
}

void QuickStatePanel::positionWidgets() {
  for (int i = 0; i < entries.size(); i++) {
    addWidget(*entries[i], i, 0);
  }

  runSizer();
}

void QuickStatePanel::executeShortcut(QuickStateEntry *entry,
                                      Singleton *singleton) {
  singleton->frameManager()->mainView()->previewPanel()->setToPresenters(entry);
}

void QuickStatePanel::setShortcut(QuickStateEntry *entry,
                                  Singleton *singleton) {
  singleton->frameManager()
      ->mainView()
      ->controlPanel()
      ->updateScreenTextFromSelected(entry);
}

}  // namespace cszb_scoreboard
