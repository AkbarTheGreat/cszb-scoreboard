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

#include <wx/dialog.h>    // for wxDEFAULT_DIALOG_STYLE
#include <wx/gdicmn.h>    // for wxPoint, wxSize
#include <wx/string.h>    // for wxString
#include <wx/tbarbase.h>  // for wxDefaultPosition

#include <cstdint>  // for int64_t
#include <memory>   // for unique_ptr
#include <string>   // for string

#include "config/Position.h"                    // for Size
#include "config/swx/defs.h"                    // for wxID_ANY, wxWindowID
#include "ui/widget/Widget.h"                   // for Widget
#include "ui/widget/swx/Panel.h"                // for Panel
#include "ui/widget/swx/PropertySheetDialog.h"  // for PropertySheetDialog
#include "wx/colour.h"                          // for wxColour
#include "wx/window.h"                          // for wxWindow

namespace cszb_scoreboard {

class ScrollingPanel;
class Button;
class CheckBox;
class ColorPicker;
class Divider;
class FilePicker;
class Label;
class ListBox;
class RadioButton;
class Radio;
class SearchBox;
class Text;
class Toggle;

class Panel : public Widget {
 public:
  explicit Panel(swx::Panel *panel) { _wx = panel; }
  virtual ~Panel();

  // Methods to create internal widgets, like buttons or labels.
  [[nodiscard]] auto button(const std::string &label,
                            bool exact_fit = false) const
      -> std::unique_ptr<Button>;
  [[nodiscard]] auto checkBox(const std::string &label) const
      -> std::unique_ptr<CheckBox>;
  [[nodiscard]] auto colorPicker(const wxColour &initial_color) const
      -> std::unique_ptr<ColorPicker>;
  [[nodiscard]] auto divider() const -> std::unique_ptr<Divider>;
  [[nodiscard]] auto openFilePicker(const std::string &title,
                                    const std::string &selectionMode) const
      -> std::unique_ptr<FilePicker>;
  [[nodiscard]] auto label(const std::string &text) const
      -> std::unique_ptr<Label>;
  [[nodiscard]] auto listBox(const std::string &title) const
      -> std::unique_ptr<ListBox>;
  [[nodiscard]] auto radio(const std::string &name, const char *const *choices,
                           int64_t choices_size, bool is_vertical = true) const
      -> std::unique_ptr<Radio>;
  [[nodiscard]] auto radioButton() const -> std::unique_ptr<RadioButton>;
  [[nodiscard]] auto scrollingPanel() const -> std::unique_ptr<ScrollingPanel>;
  [[nodiscard]] auto searchBox(const std::string &initial_text) const
      -> std::unique_ptr<SearchBox>;
  [[nodiscard]] auto text(const std::string &initial_text,
                          bool multi_line = false) const
      -> std::unique_ptr<Text>;
  [[nodiscard]] auto toggle(const std::string &label) const
      -> std::unique_ptr<Toggle>;

  // Create an anonymous Panel child for the current one, akin to button(),
  // above.
  [[nodiscard]] auto panel(bool self_managed = false) const
      -> std::unique_ptr<Panel>;

  // Create a child wx entry, for new Dialog object creation.
  [[nodiscard]] auto childDialog(const wxString &title,
                                 wxWindowID id = wxID_ANY,
                                 const wxPoint &pos = wxDefaultPosition,
                                 const wxSize &size = wxDefaultSize,
                                 int64_t style = wxDEFAULT_DIALOG_STYLE,
                                 const wxString &name = wxDialogNameStr) const
      -> swx::PropertySheetDialog * {
    return new swx::PropertySheetDialog(wx(), id, title, pos, size, style, name);
  }
  // Create a child wx entry, for new Panel object creation.
  [[nodiscard]] auto childPanel() const -> swx::Panel *;
  // Create a child wx entry, sized as large as the given rectangle.
  [[nodiscard]] auto childPanel(int width, int height) const -> swx::Panel *;
  void hide() const { wx()->Hide(); }
  void refresh() const { wx()->Refresh(); }
  // TODO(akbar): Get rid of this duplicate setSize method
  void setSize(const wxSize &size) const { wx()->SetSize(size); }
  void setSize(const Size &size) const { wx()->SetSize(size.toWx()); }
  void show() const { wx()->Show(); }
  // Temporary -- remove once we get rid of clients using wx_size
  [[nodiscard]] auto wx_size() const -> wxSize { return wx()->GetSize(); }
  [[nodiscard]] auto size() const -> Size;
  void toolTip(const std::string &tip) const { wx()->SetToolTip(tip); }
  void update() const { wx()->Update(); }

  // TODO(akbar): make this private once construction is all moved away from
  // passing wx widgets along.
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 protected:
  // If true, this panel will destory its own wxPanel object rather than rely
  // on the parent wxWidget to do it for us.
  bool should_self_delete = false;

  swx::Panel *_wx;
};

}  // namespace cszb_scoreboard
