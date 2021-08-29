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

#include <wx/arrstr.h>    // for wxArrayString
#include <wx/listctrl.h>  // IWYU pragma: keep for wxListCtrl
#include <wx/string.h>    // for wxString
// IWYU pragma: no_include <wx/generic/listctrl.h>

namespace cszb_scoreboard {

auto ListBox::listSize() -> int64_t { return strings().size(); }

auto ListBox::selectedIndex() -> int64_t {
  if (listSize() == 0) {
    return -1;
  }
  return wx->GetListCtrl()->GetNextItem(-1, wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
}

void ListBox::selectItem(int64_t select_index) {
  if (listSize() <= select_index) {
    select_index = 0;
  }

  wx->GetListCtrl()->SetItemState(select_index, wxLIST_STATE_SELECTED,
                                  wxLIST_STATE_SELECTED);
}

auto ListBox::strings() -> std::vector<std::string> {
  std::vector<std::string> strings;
  wxArrayString wx_strings;
  wx->GetStrings(wx_strings);
  for (const auto &entry : wx_strings) {
    strings.push_back(entry.ToStdString());
  }
  return strings;
}

void ListBox::setStrings(const std::vector<std::string> &strings) {
  wxArrayString wx_strings;
  for (const auto &entry : strings) {
    wx_strings.Add(entry.c_str());
  }
  wx->SetStrings(wx_strings);
}

}  // namespace cszb_scoreboard
