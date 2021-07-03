/*
ui/widget/Notebook.cpp: A notebook is a panel that holds other panels in a tab
navigation style.

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

#include "ui/widget/Notebook.h"

namespace cszb_scoreboard {

void Notebook::addToSizer(wxGridBagSizer *sizer, int row, int column,
                          int row_span, int column_span, int border_size,
                          int flag) {
  sizer->Add(wx, wxGBPosition(row, column), wxGBSpan(row_span, column_span),
             flag, border_size);
}

}  // namespace cszb_scoreboard
