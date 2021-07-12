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
#include <wx/event.h>
#include <vector>
#include <memory>
#include <string>

#include "ScoreboardCommon.h"
#include "config.pb.h"
#include "ui/dialog/settings/SettingsPage.h"
#include "ui/widget/Frame.h"
#include "ui/widget/TabbedDialog.h"

namespace cszb_scoreboard {
class Frame;

namespace swx {
class PropertySheetDialog;
}  // namespace swx

extern const wxEventTypeTag<wxCommandEvent> SETTINGS_UPDATED;

class SettingsDialog : public TabbedDialog {
 public:
  SettingsDialog(swx::PropertySheetDialog *wx, Frame *parent);

 private:
  void addPage(std::unique_ptr<SettingsPage> page, const std::string &name);
  void bindEvents();
  void saveSettings();
  auto validateSettings() -> bool;
  void onOk();
  void onCancel();
  void onClose();

  Frame *parent;
  std::vector<std::unique_ptr<SettingsPage>> pages;
};

}  // namespace cszb_scoreboard
