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

#include <wx/bookctrl.h>

#include "ScoreboardCommon.h"
#include "ui/dialog/settings/DisplaySettingsPage.h"
#include "ui/dialog/settings/TeamSettingsPage.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

wxDEFINE_EVENT(SETTINGS_UPDATED, wxCommandEvent);

auto SettingsDialog::Create(wxWindow* parent) -> bool {
  this->parent = parent;
  if (!wxPropertySheetDialog::Create(parent, wxID_ANY, "Scoreboard Settings")) {
    return false;
  }
  CreateButtons(wxOK | wxCANCEL);
  addPage(new TeamSettingsPage(GetBookCtrl()), "Teams");
  addPage(new DisplaySettingsPage(GetBookCtrl()), "Displays");
  LayoutDialog();
  bindEvents();
  return true;
}

void SettingsDialog::addPage(SettingsPage* page, const std::string& name) {
  pages.push_back(page);
  GetBookCtrl()->AddPage(page, name);
}

void SettingsDialog::bindEvents() {
  Bind(wxEVT_BUTTON, &SettingsDialog::onOk, this, wxID_OK);
  Bind(wxEVT_BUTTON, &SettingsDialog::onCancel, this, wxID_CANCEL);
  Bind(wxEVT_CLOSE_WINDOW, &SettingsDialog::onClose, this);
}

void SettingsDialog::onOk(wxCommandEvent& event) {
  if (validateSettings()) {
    saveSettings();
    wxCommandEvent settings_updated_event(SETTINGS_UPDATED);
    // Normally, I'd use wxPostEvent here for asynchronous event handling, but
    // it sometimes doesn't work (Maybe due to a race condition with object
    // destruction?)  Processing synchronously here is our best compromise.
    ProcessEvent(settings_updated_event);
    Close(true);
    return;
  }
}

void SettingsDialog::onCancel(wxCommandEvent& event) { Close(true); }

void SettingsDialog::onClose(wxCloseEvent& event) {
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog.  To top that off, if you set focus
  // before calling Destroy(), things quit working.  But Destroying calls the
  // destructor, so we can't rely on this->parent anymore after Destroy is
  // called.  So we save it in a local pointer temporarily for this purpose.
  wxWindow* local_parent = parent;
  Destroy();
  local_parent->SetFocus();
}

auto SettingsDialog::validateSettings() -> bool {
  for (auto* page : pages) {
    if (!page->validateSettings()) {
      return false;
    }
  }
  return true;
}

void SettingsDialog::saveSettings() {
  for (auto* page : pages) {
    page->saveSettings();
  }
}

}  // namespace cszb_scoreboard
