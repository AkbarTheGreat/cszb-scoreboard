/*
ui/widget/ListBox.h: A list of strings which can be reordered and edited.

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

#include <wx/bmpbuttn.h>  // IWYU pragma: keep for wxBitmapButton

#include <cstdint>     // for int64_t
#include <functional>  // for function
#include <string>      // for string
#include <vector>      // for vector

#include "config/swx/defs.h"                // for wxID_ANY
#include "config/swx/event.h"               // for wxEventTypeTag
#include "ui/widget/Widget.h"               // for Widget
#include "ui/widget/swx/EditableListBox.h"  // for EditableListBox
#include "wx/window.h"                      // for wxWindow
// IWYU pragma: no_include <wx/gtk/bmpbuttn.h>

class wxWindow;

namespace cszb_scoreboard {

class ListBox : public Widget {
 public:
  explicit ListBox(swx::EditableListBox *list_box) { _wx = list_box; }

  void bind(const wxEventTypeTag<wxListEvent> &eventType,
            const std::function<void(wxListEvent &)> &lambda,
            int id = wxID_ANY) {
    wx()->Bind(eventType, lambda, id);
  }

  // Bind an event against the "New" button on this list.
  void bindNew(const wxEventTypeTag<wxCommandEvent> &eventType,
               const std::function<void(wxCommandEvent &)> &lambda,
               int id = wxID_ANY) {
    _wx->GetNewButton()->Bind(eventType, lambda, id);
  }
  auto listSize() -> int64_t;
  auto selectedIndex() -> int64_t;
  void selectItem(int64_t select_index);

  auto strings() -> std::vector<std::string>;
  void setStrings(const std::vector<std::string> &strings);

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx; }

 private:
  swx::EditableListBox *_wx;
};

}  // namespace cszb_scoreboard
