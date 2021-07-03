/*
ui/widget/Notebook.h: A notebook is a panel that holds other panels in a tab
navigation style.

Copyright 2021 Tracy Beck

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

#include <wx/gbsizer.h>

#include "ScoreboardCommon.h"
#include "ui/widget/swx/Notebook.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class Notebook {
 public:
  explicit Notebook(swx::Notebook *notebook) { wx = notebook; }
  void focus() { wx->SetFocus(); };
  void addToSizer(wxGridBagSizer *sizer, int row, int column, int row_span = 1,
                  int column_span = 1, int border_size = DEFAULT_BORDER_SIZE,
                  int flag = wxALL | wxGROW);
  void bind(const wxEventTypeTag<wxAuiNotebookEvent> &eventType,
            const std::function<void(wxAuiNotebookEvent &)> &lambda) {
    wx->Bind(eventType, lambda);
  }

  void AddPage(wxWindow *page, const wxString &caption) {
    wx->AddPage(page, caption);
  }
  int GetSelection() { return wx->GetSelection(); }

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Notebook *wx;
};

}  // namespace cszb_scoreboard
