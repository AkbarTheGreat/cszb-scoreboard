/*
ui/dialog/SettingsDialog.cpp: A settings dialog for configuring various aspects
of the scoreboard which are then saved as a part of the configuration protobuf.

Copyright 2019-2022 Tracy Beck

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

#include <wx/defs.h>  // for wxID_CANCEL, wxI...

#include <algorithm>
#include <utility>  // for move

#include "ScoreboardCommon.h"                        // for DEFAULT_BORDER_SIZE
#include "ui/dialog/settings/DisplaySettingsPage.h"  // for DisplaySettingsPage
#include "ui/dialog/settings/TeamSettingsPage.h"     // for TeamSettingsPage
#include "ui/frame/MainView.h"

namespace cszb_scoreboard {
namespace swx {
class PropertySheetDialog;
}  // namespace swx

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

wxDEFINE_EVENT(SETTINGS_UPDATED, wxCommandEvent);

SettingsDialog::SettingsDialog(swx::PropertySheetDialog *wx, MainView *parent)
    : TabbedDialog(wx) {
  this->parent = parent;
  addPage(std::make_unique<TeamSettingsPage>(childPanel()), "Teams");
  addPage(std::make_unique<DisplaySettingsPage>(childPanel()), "Displays");
  runSizer();
  bindEvents();
}

void SettingsDialog::addPage(std::unique_ptr<SettingsPage> page,
                             const std::string &name) {
  TabbedDialog::addPage(*page, name);
  pages.push_back(std::move(page));
}

void SettingsDialog::bindEvents() {
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onOk(); },
      wxID_OK);
  bind(
      wxEVT_BUTTON, [this](wxCommandEvent &event) -> void { this->onCancel(); },
      wxID_CANCEL);
  MainView *local_parent = parent;
  bind(wxEVT_CLOSE_WINDOW, [local_parent](wxCloseEvent &event) -> void {
    local_parent->onSettingsClose();
  });
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

auto SettingsDialog::validateSettings() -> bool {
  return std::all_of(pages.begin(), pages.end(),
                     [](const std::unique_ptr<SettingsPage> &page) -> bool {
                       return page->validateSettings();
                     });
}

void SettingsDialog::saveSettings() {
  for (const auto &page : pages) {
    page->saveSettings();
  }
}

}  // namespace cszb_scoreboard
