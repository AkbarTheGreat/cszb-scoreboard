/*
ui/widget/TabbedDialog.h: A substantial pop-up dialog, which contains controls
within tabs.

Copyright 2021-2022 Tracy Beck

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

#include <wx/gdicmn.h>    // for wxPoint, wxSize (ptr ...
#include <wx/panel.h>     // for wxPanelNameStr
#include <wx/string.h>    // for wxString
#include <wx/tbarbase.h>  // for wxDefaultPosition

#include <cstdint>  // for int64_t
#include <memory>   // for unique_ptr
#include <string>   // for string

#include "config/swx/defs.h"                    // for wxCANCEL, wxID_ANY, wxOK
#include "ui/widget/Widget.h"                   // for Widget
#include "ui/widget/swx/Panel.h"                // for PanelImpl, Panel (ptr...
#include "ui/widget/swx/PropertySheetDialog.h"  // for PropertySheetDialog
#include "wx/notebook.h"                        // IWYU pragma: keep
#include "wx/window.h"                          // for wxWindow
// IWYU pragma: no_include <wx/bookctrl.h>

class wxEvent;

namespace cszb_scoreboard {

class Panel;

class TabbedDialog : public Widget {
 public:
  explicit TabbedDialog(swx::PropertySheetDialog *dialog,
                        int64_t buttons = wxOK | wxCANCEL);
  virtual ~TabbedDialog();

  void addPage(const Panel &page, const std::string &name);
  [[nodiscard]] auto childPanel(wxWindowID id = wxID_ANY,
                                const wxPoint &pos = wxDefaultPosition,
                                const wxSize &size = wxDefaultSize,
                                int64_t style = wxTAB_TRAVERSAL,
                                const wxString &name = wxPanelNameStr) const
      -> swx::Panel * {
    return new swx::PanelImpl(_wx->GetBookCtrl(), id, pos, size, style, name);
  }
  void close(bool force = true) { wx()->Close(force); }
  [[nodiscard]] auto panel() const -> std::unique_ptr<Panel>;
  void runSizer() { _wx->LayoutDialog(); }
  void show() { wx()->Show(); }
  void sendEvent(wxEvent *event) { _wx->ProcessEvent(*event); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::PropertySheetDialog *_wx;
};

}  // namespace cszb_scoreboard
