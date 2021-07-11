/*
ui/dialog/SettingsDialog.cpp: A settings dialog for configuring various aspects
of the scoreboard which are then saved as a part of the configuration protobuf.

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

#include "ui/dialog/SettingsDialog.h"

#include "ScoreboardCommon.h"
#include "ui/dialog/settings/DisplaySettingsPage.h"
#include "ui/dialog/settings/TeamSettingsPage.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

wxDEFINE_EVENT(SETTINGS_UPDATED, wxCommandEvent);

SettingsDialog::SettingsDialog(swx::PropertySheetDialog* wx, Frame* parent)
    : TabbedDialog(wx) {
  this->parent = parent;
  addPage(std::make_unique<TeamSettingsPage>(childPanel()), "Teams");
  addPage(std::make_unique<DisplaySettingsPage>(childPanel()), "Displays");
  runSizer();
  bindEvents();
}

void SettingsDialog::addPage(std::unique_ptr<SettingsPage> page,
                             const std::string& name) {
  TabbedDialog::addPage(*page, name);
  pages.push_back(std::move(page));
}

void SettingsDialog::bindEvents() {
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent& event) -> void { this->onOk(); },
      wxID_OK);
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent& event) -> void { this->onCancel(); },
      wxID_CANCEL);
  bind(wxEVT_CLOSE_WINDOW,
       [this](wxCloseEvent& event) -> void { this->onClose(); });
}

void SettingsDialog::onOk() {
  if (validateSettings()) {
    saveSettings();
    wxCommandEvent settings_updated_event(SETTINGS_UPDATED);
    // Normally, I'd use wxPostEvent here for asynchronous event handling, but
    // it sometimes doesn't work (Maybe due to a race condition with object
    // destruction?)  Processing synchronously here is our best compromise.
    sendEvent(&settings_updated_event);
    close();
    return;
  }
}

void SettingsDialog::onCancel() { close(true); }

void SettingsDialog::onClose() {
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog.  To top that off, if you set focus
  // before calling Destroy(), things quit working.  But Destroying calls the
  // destructor, so we can't rely on this->parent anymore after Destroy is
  // called.  So we save it in a local pointer temporarily for this purpose.
  Frame* local_parent = parent;
  selfDestruct();
  local_parent->focus();
}

auto SettingsDialog::validateSettings() -> bool {
  for (const auto& page : pages) {
    if (!page->validateSettings()) {
      return false;
    }
  }
  return true;
}

void SettingsDialog::saveSettings() {
  for (const auto& page : pages) {
    page->saveSettings();
  }
}

}  // namespace cszb_scoreboard
