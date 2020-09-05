/*
ui/UiUtil.cpp: Some common ui-specific utility methods collected here to reduce
duplication.

Copyright 2020 Tracy Beck

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

#include "ui/UiUtil.h"

namespace cszb_scoreboard {

wxSizer* UiUtil::sizer(long rows, long columns) {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(rows, columns, 0, 0);
  sizer->SetFlexibleDirection(wxBOTH);
  sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
  return sizer;
}

void UiUtil::addToGridBag(wxGridBagSizer* sizer, wxWindow* item, int row,
                          int column, int row_span, int column_span,
                          int border_size) {
  sizer->Add(item, wxGBPosition(row, column), wxGBSpan(row_span, column_span),
             wxALL, border_size);
}

}  // namespace cszb_scoreboard
