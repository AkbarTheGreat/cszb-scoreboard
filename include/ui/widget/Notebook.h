/*
ui/widget/Notebook.h: A notebook is a panel that holds other panels in a tab
navigation style.

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

#pragma once

#include <wx/gbsizer.h>

#include "ScoreboardCommon.h"
#include "ui/widget/Panel.h"
#include "ui/widget/Widget.h"
#include "ui/widget/swx/Notebook.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class Notebook : public Widget {
 public:
  explicit Notebook(swx::Notebook *notebook) { _wx = notebook; }
  void addTab(const Panel &tab, const std::string &name) const {
    _wx->AddPage(tab.wx(), name);
  }
  void addToSizer(wxGridBagSizer *sizer, int row, int column, int row_span = 1,
                  int column_span = 1, int border_size = DEFAULT_BORDER_SIZE,
                  int flag = wxALL | wxGROW);
  void bind(const wxEventTypeTag<wxAuiNotebookEvent> &eventType,
            const std::function<void(wxAuiNotebookEvent &)> &lambda) const {
    wx()->Bind(eventType, lambda);
  }
  auto childPanel(wxWindowID id = wxID_ANY,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  int64_t style = wxTAB_TRAVERSAL,
                  const wxString &name = wxPanelNameStr) -> swx::Panel * {
    return new swx::PanelImpl(wx(), id, pos, size, style, name);
  }
  void focus() const { wx()->SetFocus(); };
  [[nodiscard]] auto selection() const -> int { return _wx->GetSelection(); }
  void setSelection(int selection) { _wx->SetSelection(selection); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::Notebook *_wx;
};

}  // namespace cszb_scoreboard
