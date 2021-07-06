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
#include "ui/widget/Widget.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

class Panel : public Widget {
 public:
  explicit Panel(swx::Panel *panel) { wx = panel; }
  virtual ~Panel();

  [[nodiscard]] auto childPanel(wxWindowID id = wxID_ANY,
                                const wxPoint &pos = wxDefaultPosition,
                                const wxSize &size = wxDefaultSize,
                                long style = wxTAB_TRAVERSAL,
                                const wxString &name = wxPanelNameStr) const
      -> swx::Panel * {
    return new swx::Panel(wx, id, pos, size, style, name);
  }
  void hide() { wx->Hide(); }
  void refresh() const { wx->Refresh(); }
  void setSize(const wxSize &size) const { wx->SetSize(size); }
  void show() { wx->Show(); }
  void toolTip(const std::string &tip) { wx->SetToolTip(tip); }

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Panel *wx;

 protected:
  void update() const { wx->Update(); }
  [[nodiscard]] auto size() const -> wxSize { return wx->GetSize(); }
  // If true, this panel will destory its own wxPanel object rather than rely on
  // the parent wxWidget to do it for us.
  bool should_self_delete = false;

  auto _wx() -> wxWindow * { return wx; }
};

}  // namespace cszb_scoreboard
