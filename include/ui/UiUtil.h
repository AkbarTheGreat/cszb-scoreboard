/*
ui/UiUtil.h: Some common ui-specific utility methods collected here to reduce
duplication.

Copyright 2020-2023 Tracy Beck

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

#include <wx/defs.h>  // for wxALL, wxGROW

#include <cstdint>  // for int64_t

#include "ScoreboardCommon.h"  // for DEFAULT_BORDER_SIZE

class wxGridBagSizer;
class wxSizer;
class wxWindow;

namespace cszb_scoreboard {

class UiUtil {
 public:
  // If making a new sizer, prefer using a wxGridBagSizer directly (it has a
  // simple constructor, so it doesn't need this helper method at all)
  static auto sizer(int64_t rows, int64_t columns) -> wxSizer *;
  static void addToGridBag(wxGridBagSizer *sizer, wxWindow *item, int row,
                           int column, int row_span = 1, int column_span = 1,
                           int border_size = DEFAULT_BORDER_SIZE,
                           int flags = wxALL | wxGROW);
};

}  // namespace cszb_scoreboard
