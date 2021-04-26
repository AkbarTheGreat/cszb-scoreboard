/*
ui/dialog/SettingsDialog.h: A settings dialog for configuring various aspects of
the scoreboard which are then saved as a part of the configuration protobuf.

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
#pragma once

#include <wx/propdlg.h>
#include <wx/wx.h>

#include <vector>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "ui/dialog/settings/SettingsPage.h"

namespace cszb_scoreboard {

extern const wxEventTypeTag<wxCommandEvent> SETTINGS_UPDATED;

class SettingsDialog : public wxPropertySheetDialog {
 public:
  auto Create(wxWindow *parent) -> bool;

 private:
  void addPage(SettingsPage *page, const std::string &name);
  void bindEvents();
  void saveSettings();
  auto validateSettings() -> bool;
  std::vector<SettingsPage *> pages;
  wxWindow *parent;
  // wxWidgets callbacks, waive linting error for references.
  void onOk(wxCommandEvent &event);      // NOLINT(google-runtime-references)
  void onCancel(wxCommandEvent &event);  // NOLINT(google-runtime-references)
  void onClose(wxCloseEvent &event);     // NOLINT(google-runtime-references)
};

}  // namespace cszb_scoreboard
