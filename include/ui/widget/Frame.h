/*
ui/widget/Frame.h: A frame represents a free-standing window that is not a
dialog.

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

#include <stdint.h>          // for int64_t
#include <wx/accel.h>        // IWYU pragma: keep for wxAcceleratorTable
#include <wx/aui/auibook.h>  // for wxAUI_NB_SCROLL_BUTTONS
#include <wx/dialog.h>       // for wxDEFAULT_DIALOG_STYLE
#include <wx/gdicmn.h>       // for wxPoint, wxSize (ptr ...
#include <wx/panel.h>        // for wxPanelNameStr
#include <wx/string.h>       // for wxString
#include <wx/tbarbase.h>     // for wxDefaultPosition

#include <optional>  // for optional
#include <string>    // for string
#include <vector>    // for vector

#include "config/Position.h"                    // for Position, Size
#include "config/swx/defs.h"                    // for wxID_ANY, wxWindowID
#include "ui/widget/Widget.h"                   // for Widget
#include "ui/widget/swx/Frame.h"                // for Frame
#include "ui/widget/swx/Notebook.h"             // for Notebook
#include "ui/widget/swx/Panel.h"                // for Panel
#include "ui/widget/swx/PropertySheetDialog.h"  // for PropertySheetDialog
#include "wx/window.h"                          // for wxWindow

class wxAcceleratorEntry;
class wxWindow;

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
      : Frame(new swx::Frame(nullptr, wxID_ANY, title, pos.toWx(),
                             size.toWx())) {}
  // Injectable constructor, for testing.
  explicit Frame(swx::Frame *frame) { wx = frame; }
  virtual ~Frame() { _wx()->Destroy(); }

  void installHotkeys(const std::vector<wxAcceleratorEntry> &keys) {
    wx->SetAcceleratorTable(wxAcceleratorTable(keys.size(), keys.data()));
  }
  auto position() -> wxPoint { return wx->GetPosition(); }
  void setStatusBar(const wxString &text);
  auto show(bool show) -> bool { return wx->Show(show); }
  void updateWindow() { wx->Update(); }
  void closeWindow() { wx->Close(true); }

 protected:
  void alwaysOnTop(bool isOnTop);
  void fullScreen(bool show) { wx->ShowFullScreen(show); }
  void minimize() { wx->Iconize(); }
  void menuBar(const std::vector<MenuCategory> &menu);
  void setDimensions(const Position &position, const Size &size);

  auto childDialog(const wxString &title, wxWindowID id = wxID_ANY,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   int64_t style = wxDEFAULT_DIALOG_STYLE,
                   const wxString &name = wxDialogNameStr)
      -> swx::PropertySheetDialog * {
    return new swx::PropertySheetDialog(wx, id, title, pos, size, style, name);
  }
  auto childPanel(wxWindowID id = wxID_ANY,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  int64_t style = wxTAB_TRAVERSAL,
                  const wxString &name = wxPanelNameStr) -> swx::Panel * {
    return new swx::Panel(wx, id, pos, size, style, name);
  }
  auto childNotebook(wxWindowID id = wxID_ANY,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     int64_t style = DEFAULT_NOTEBOOK_STYLE)
      -> swx::Notebook * {
    return new swx::Notebook(wx, id, pos, size, style);
  }

  [[nodiscard]] auto _wx() const -> wxWindow * override { return wx; }
  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Frame *wx;

 private:
  bool hasStatusBar = false;
};

}  // namespace cszb_scoreboard
