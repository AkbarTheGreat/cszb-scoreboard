/*
ui/widget/ListBox.h: A list of strings which can be reordered and edited.

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

#include <stdint.h>  // for int32_t

#include <functional>  // for function

#include "config/swx/defs.h"                // for wxID_ANY
#include "config/swx/event.h"               // for wxEventTypeTag, wxCommand...
#include "ui/widget/Widget.h"               // for Widget
#include "ui/widget/swx/EditableListBox.h"  // for EditableListBox

class wxWindow;
class wxArrayString;

namespace cszb_scoreboard {

class ListBox : public Widget {
 public:
  explicit ListBox(swx::EditableListBox *list_box) { wx = list_box; }

  void bind(const wxEventTypeTag<wxListEvent> &eventType,
            const std::function<void(wxListEvent &)> &lambda,
            int id = wxID_ANY) {
    wx->Bind(eventType, lambda, id);
  }

  // Bind an event against the "New" button on this list.
  void bindNew(const wxEventTypeTag<wxCommandEvent> &eventType,
               const std::function<void(wxCommandEvent &)> &lambda,
               int id = wxID_ANY) {
    wx->GetNewButton()->Bind(eventType, lambda, id);
  }
  auto listSize() -> int32_t;
  auto selectedIndex() -> int32_t;
  void selectItem(int32_t select_index);

  auto strings() -> std::vector<std::string>;
  void setStrings(const std::vector<std::string> &strings);

 protected:
  auto _wx() const -> wxWindow * override { return wx; }

 private:
  swx::EditableListBox *wx;
};

}  // namespace cszb_scoreboard
