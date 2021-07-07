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
#include "ui/widget/Button.h"
#include "ui/widget/ColorPicker.h"
#include "ui/widget/Label.h"
#include "ui/widget/SearchBox.h"
#include "ui/widget/Text.h"
#include "ui/widget/Toggle.h"
#include "ui/widget/Widget.h"
#include "ui/widget/swx/Panel.h"

namespace cszb_scoreboard {

// pre-declare ScrollingPanel to avoid circular dependencies
class ScrollingPanel;

class Panel : public Widget {
 public:
  explicit Panel(swx::Panel *panel) { wx = panel; }
  virtual ~Panel();

  // Methods to create internal widgets, like buttons or labels.
  [[nodiscard]] auto button(const std::string &label,
                            bool exact_fit = false) const
      -> std::unique_ptr<Button>;
  [[nodiscard]] auto colorPicker(const wxColour &initial_color) const
      -> std::unique_ptr<ColorPicker>;
  [[nodiscard]] auto label(const std::string &text) const
      -> std::unique_ptr<Label>;
  [[nodiscard]] auto scrollingPanel(long scroll_style = wxHSCROLL |
                                                        wxVSCROLL) const
      -> std::unique_ptr<ScrollingPanel>;
  [[nodiscard]] auto searchBox(const std::string &initial_text) const
      -> std::unique_ptr<SearchBox>;
  [[nodiscard]] auto text(const std::string &initial_text,
                          bool multi_line = false) const
      -> std::unique_ptr<Text>;
  [[nodiscard]] auto toggle(const std::string &label) const
      -> std::unique_ptr<Toggle>;

  // Create an anonymous Panel child for the current one, akin to button(),
  // above.
  [[nodiscard]] auto panel() const -> std::unique_ptr<Panel>;

  // Create a child wx entry, for new Panel object creation.
  [[nodiscard]] auto childPanel(wxWindowID id = wxID_ANY,
                                const wxPoint &pos = wxDefaultPosition,
                                const wxSize &size = wxDefaultSize,
                                long style = wxTAB_TRAVERSAL,
                                const wxString &name = wxPanelNameStr) const
      -> swx::Panel * {
    return new swx::Panel(_wx(), id, pos, size, style, name);
  }
  void hide() const { _wx()->Hide(); }
  void refresh() const { _wx()->Refresh(); }
  void setSize(const wxSize &size) const { _wx()->SetSize(size); }
  void show() const { _wx()->Show(); }
  [[nodiscard]] auto size() const -> wxSize { return _wx()->GetSize(); }
  void toolTip(const std::string &tip) const { _wx()->SetToolTip(tip); }
  void update() const { _wx()->Update(); }

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  swx::Panel *wx;

 protected:
  // If true, this panel will destory its own wxPanel object rather than rely on
  // the parent wxWidget to do it for us.
  bool should_self_delete = false;

  [[nodiscard]] auto _wx() const -> wxWindow * override { return wx; }
};

}  // namespace cszb_scoreboard
