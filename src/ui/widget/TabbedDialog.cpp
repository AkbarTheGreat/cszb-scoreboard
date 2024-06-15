/*
ui/widget/TabbedDialog.cpp: A substantial pop-up dialog, which contains controls
within tabs.

Copyright 2021-2024 Tracy Beck

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

#include "ui/widget/TabbedDialog.h"

#include "ui/widget/Panel.h"

namespace cszb_scoreboard {

TabbedDialog::TabbedDialog(swx::PropertySheetDialog *dialog, int64_t buttons) {
  _wx = dialog;
  _wx->CreateButtons(buttons);
}

TabbedDialog::~TabbedDialog() { wx()->Destroy(); }

void TabbedDialog::addPage(const Panel &page, const std::string &name) {
  _wx->GetBookCtrl()->AddPage(page.wx(), name);
}

auto TabbedDialog::panel() const -> std::unique_ptr<Panel> {
  return std::make_unique<Panel>(childPanel());
}

}  // namespace cszb_scoreboard
