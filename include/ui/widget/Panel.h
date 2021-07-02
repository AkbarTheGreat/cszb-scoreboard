/*
ui/widget/Panel.h: A panel represents an interior panel container that is held
within other frames or panels.

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
#include <wx/wx.h>

#include <vector>

#include "ScoreboardCommon.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class Panel {
 public:
  explicit Panel(swx::Panel *panel) { wx = panel; }

  void addToSizer(wxGridBagSizer *sizer, int row, int column, int row_span = 1,
                  int column_span = 1, int border_size = DEFAULT_BORDER_SIZE,
                  int flag = wxALL | wxGROW);
  auto childPanel(wxWindowID id = wxID_ANY,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL,
                  const wxString &name = wxPanelNameStr) -> swx::Panel * {
    return new swx::Panel(wx, id, pos, size, style, name);
  }

  void Refresh() { wx->Refresh(); }
  void SetSize(const wxSize &size) { wx->SetSize(size); }
  void Destroy() { wx->Destroy(); }
  void SetSizerAndFit(wxSizer *sizer) { wx->SetSizerAndFit(sizer); }

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Panel *wx;

  //  This is also a temporary hack while things are being reworked
  wxWindow *GetParent() { return wx->GetParent(); }

 protected:
  void Update() { wx->Update(); }
  wxSize GetSize() { return wx->GetSize(); }
};

}  // namespace cszb_scoreboard
