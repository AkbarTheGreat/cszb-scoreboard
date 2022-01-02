/*
ui/widget/Frame.h: A frame represents a free-standing window that is not a
dialog.

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

#include <wx/accel.h>        // IWYU pragma: keep
#include <wx/aui/auibook.h>  // for wxAUI_NB_SCROLL_BUTTONS
#include <wx/dialog.h>       // for wxDEFAULT_DIALOG_STYLE
#include <wx/gdicmn.h>       // for wxPoint, wxSize (ptr ...
#include <wx/panel.h>        // for wxPanelNameStr
#include <wx/string.h>       // for wxString
#include <wx/tbarbase.h>     // for wxDefaultPosition

#include <cstdint>   // for int64_t
#include <optional>  // for optional
#include <string>    // for string
#include <vector>    // for vector

#include "ScoreboardCommon.h"
#include "config/Position.h"                    // for Position, Size
#include "config/swx/defs.h"                    // for wxID_ANY, wxWindowID
#include "ui/widget/Widget.h"                   // for Widget
#include "ui/widget/swx/Frame.h"                // for Frame
#include "ui/widget/swx/Notebook.h"             // for Notebook
#include "ui/widget/swx/Panel.h"                // for Panel
#include "ui/widget/swx/PropertySheetDialog.h"  // for PropertySheetDialog
#include "wx/window.h"                          // for wxWindow
// IWYU pragma: no_include <wx/generic/accel.h>

namespace cszb_scoreboard {

const int DEFAULT_NOTEBOOK_STYLE = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT |
                                   wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;

struct MenuItem {
  int64_t id;
  std::optional<std::string> name;
  std::optional<std::string> description;
};

struct MenuCategory {
  std::string name;
  std::vector<MenuItem> items;
};

class Frame : public Widget {
 public:
  explicit Frame(const std::string &title,
                 const Position &pos = Position::fromWx(wxDefaultPosition),
                 const Size &size = Size::fromWx(wxDefaultSize))
      : Frame(new swx::FrameImpl(nullptr, wxID_ANY, title, pos.toWx(),
                                 size.toWx())) {}
  virtual ~Frame() { _wx->Destroy(); }

  void installHotkeys(const std::vector<wxAcceleratorEntry> &keys) {
    _wx->SetAcceleratorTable(wxAcceleratorTable(keys.size(), keys.data()));
  }
  auto position() -> Position { return Position::fromWx(_wx->GetPosition()); }
  void setStatusBar(const wxString &text);
  auto show(bool show) -> bool { return _wx->Show(show); }
  void updateWindow() { _wx->Update(); }
  void closeWindow() { _wx->Close(true); }

  PUBLIC_TEST_ONLY
  // Injectable constructor, for testing.
  explicit Frame(swx::Frame *frame) { _wx = frame; }

 protected:
  void alwaysOnTop(bool isOnTop);
  void fullScreen(bool show) { _wx->ShowFullScreen(show); }
  void minimize() { _wx->Iconize(); }
  void menuBar(const std::vector<MenuCategory> &menu);
  void setDimensions(const Position &position, const Size &size);

  auto childDialog(const wxString &title, wxWindowID id = wxID_ANY,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   int64_t style = wxDEFAULT_DIALOG_STYLE,
                   const wxString &name = wxDialogNameStr)
      -> swx::PropertySheetDialog * {
    return new swx::PropertySheetDialog(wx(), id, title, pos, size, style,
                                        name);
  }
  auto childPanel(wxWindowID id = wxID_ANY,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  int64_t style = wxTAB_TRAVERSAL,
                  const wxString &name = wxPanelNameStr) -> swx::Panel * {
    return new swx::Panel(wx(), id, pos, size, style, name);
  }
  auto childNotebook(wxWindowID id = wxID_ANY,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     int64_t style = DEFAULT_NOTEBOOK_STYLE)
      -> swx::Notebook * {
    return new swx::Notebook(wx(), id, pos, size, style);
  }

  [[nodiscard]] auto wx() const -> wxWindow * override {
    return dynamic_cast<swx::FrameImpl *>(_wx);
  }
  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Frame *_wx;

 private:
  bool hasStatusBar = false;
};

}  // namespace cszb_scoreboard
