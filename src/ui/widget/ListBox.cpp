/*
ui/widget/ListBox.cpp: A list of strings which can be reordered and edited.

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

#include "ui/widget/ListBox.h"

#include <wx/listctrl.h>

class wxWindow;

namespace cszb_scoreboard {

auto ListBox::listSize() -> int32_t {
  wxArrayString data;
  strings(data);
  return data.GetCount();
}

auto ListBox::selectedIndex() -> int32_t {
  if (listSize() == 0) {
    return -1;
  }
  return wx->GetListCtrl()->GetNextItem(-1, wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
}

void ListBox::selectItem(int32_t select_index) {
  if (listSize() <= select_index) {
    select_index = 0;
  }

  wx->GetListCtrl()->SetItemState(select_index, wxLIST_STATE_SELECTED,
                                  wxLIST_STATE_SELECTED);
}

}  // namespace cszb_scoreboard
